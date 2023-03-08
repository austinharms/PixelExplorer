#include "PxeEngine.h"

#include <shared_mutex>
#include <mutex>
#include <deque>
#include <unordered_map>
#include <list>
#include <thread>
#include <string>
#include <new>

#include "GL/glew.h"
#include "pvd/PxPvdTransport.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "PxeLogger.h"
#include "PxPhysicsVersion.h"
#include "extensions/PxExtensionsAPI.h"
#include "SDL.h"

namespace pxengine {
	struct PxeEngine::Impl
	{
		Impl(PxeLogInterface& log, physx::PxAssertHandler& physAssert) : _loggingInterface(log), _defaultPhysAssertHandler(physAssert) {}

		PxeLogInterface& _loggingInterface;
		PxeInputManager _inputManager;
		PxeRenderPipeline _renderPipeline;
		SDL_Window* _primarySDLWindow;
		SDL_GLContext _primaryGlContext;

		std::shared_mutex _windowsMutex;
		// Keyed by SDL window id
		std::unordered_map<uint32_t, PxeWindow*> _initializedWindows;
		std::mutex _assetMutex;
		std::deque<PxeGLAsset*> _assetQueue;
		std::shared_mutex _sceneMutex;
		std::list<PxeScene*> _scenes;

		physx::PxAssertHandler& _defaultPhysAssertHandler;
		physx::PxFoundation* _physFoundation;
#ifdef PXE_DEBUG
		physx::PxPvd* _physPVD;
		physx::PxPvdTransport* _physPVDTransport;
#endif
		physx::PxPhysics* _physPhysics;
		physx::PxCooking* _physCooking;
		physx::PxDefaultCpuDispatcher* _physDefaultDispatcher;
		physx::PxDefaultAllocator _physAllocator;
		physx::PxTolerancesScale _physScale;

		uint64_t _lastDeltaUpdate;
		float _lastDeltaTime;
		bool _shouldShutdown;
		bool _windowQueueForCreation;
		bool _initializedPrimaryGlContext;
	};

	PxeEngine* PxeEngine::s_engineInstance = nullptr;

	PXE_NODISCARD PxeEngine& PxeEngine::getInstance()
	{
		PXE_ASSERT(s_engineInstance, "Attempted to get invalid PxeEngine instance");
		return *s_engineInstance;
	}

	PxeEngine::PxeEngine(PxeLogInterface& logInterface) {
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeEngine::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeEngine::STORAGE_ALIGN need be changed");

		PXE_ASSERT(!s_engineInstance, "Attempted to create new PxeEngine when one exist");
		new(&_storage) Impl(logInterface, physx::PxGetAssertHandler());
		s_engineInstance = this; // all logging function work from this point on
		PXE_INFO("PxeEngine Created");
		Impl& imp = impl();
		imp._primarySDLWindow = nullptr;
		imp._primaryGlContext = nullptr;
		imp._physFoundation = nullptr;
#ifdef PXE_DEBUG
		imp._physPVD = nullptr;
		imp._physPVDTransport = nullptr;
#endif // PXE_DEBUG
		imp._physPhysics = nullptr;
		imp._physCooking = nullptr;
		imp._physDefaultDispatcher = nullptr;
		imp._lastDeltaUpdate = 0;
		imp._lastDeltaTime = 0;
		imp._shouldShutdown = false;
		imp._windowQueueForCreation = false;
		imp._initializedPrimaryGlContext = false;

		initSDL();
		initPhysics();
		PXE_INFO("PxeEngine initialized");
	}

	PxeEngine::~PxeEngine()
	{
		deinitPhysics();
		deinitSDL();
		PXE_INFO("PxeEngine uninitialized");
		s_engineInstance = nullptr; // no logging function work from this point on
		impl().~Impl(); 
	}

	void PxeEngine::initPhysics()
	{
		if (std::thread::hardware_concurrency() <= 0) {
			PXE_WARN("Failed to get hardware thread count");
		}

		physx::PxSetAssertHandler(*this);
		Impl& imp = impl();
		imp._physScale.length = 1;
		imp._physScale.speed = 9.81f;
		imp._physFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, imp._physAllocator, *this);
		if (!imp._physFoundation) {
			PXE_FATAL("Failed to create PxeEngine's physx::PxFoundation");
		}

#ifdef PXE_DEBUG
		imp._physPVD = PxCreatePvd(*imp._physFoundation);
		if (!imp._physPVD) {
			PXE_FATAL("Failed to create PxeEngine's physx::PxPvd");
		}

		imp._physPVDTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 1000);
		if (imp._physPVDTransport) {
			PXE_INFO("PhysX PVD socket created 127.0.0.1:5425");
		}
		else {
			PXE_FATAL("Failed to create PxeEngine's physx::PxPvdTransport");
		}

		imp._physPVD->connect(*imp._physPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
#define PXE_PHYSICS_PVD_POINTER imp._physPVD
#define PXE_PHYSICS_TRACK_ALLOCATIONS true
#else
#define PXE_PHYSICS_PVD_POINTER nullptr
#define PXE_PHYSICS_TRACK_ALLOCATIONS false
#endif
		imp._physPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *imp._physFoundation, imp._physScale, PXE_PHYSICS_TRACK_ALLOCATIONS, PXE_PHYSICS_PVD_POINTER);
		if (!imp._physPhysics) {
			PXE_FATAL("Failed to create PxeEngine's physx::PxPhysics");
		}

		uint32_t threadCount = std::thread::hardware_concurrency() / 2;
		if (threadCount < 2) {
			PXE_WARN("Calculated thread pool size less then 2, using 2 threads for physx::PxDefaultCpuDispatcher");
			threadCount = 2;
		}
		else {
			PXE_INFO("Using " + std::to_string(threadCount) + " threads for physx::PxDefaultCpuDispatcher");
		}

		imp._physDefaultDispatcher = physx::PxDefaultCpuDispatcherCreate(threadCount);
		if (!imp._physDefaultDispatcher) {
			PXE_FATAL("Failed to create PxeEngine's physx::PxDefaultCpuDispatcher");
		}

		physx::PxCookingParams params(imp._physScale);
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		imp._physCooking = PxCreateCooking(PX_PHYSICS_VERSION, *imp._physFoundation, params);
		if (!imp._physCooking) {
			PXE_FATAL("Failed to create PxeEngine's physx::PxCooking");
		}

		if (!PxInitExtensions(*imp._physPhysics, PXE_PHYSICS_PVD_POINTER)) {
			PXE_FATAL("Failed to init PhysX extensions");
		}

		PXE_INFO("PhysX initialized");
	}

	void PxeEngine::deinitPhysics()
	{
		Impl& imp = impl();
		PxCloseExtensions();
		imp._physDefaultDispatcher->release();
		imp._physCooking->release();
		imp._physPhysics->release();
#ifdef PXE_DEBUG
		imp._physPVD->disconnect();
		imp._physPVD->release();
		imp._physPVDTransport->release();
#endif
		imp._physFoundation->release();
		physx::PxSetAssertHandler(imp._defaultPhysAssertHandler);
		PXE_INFO("PhysX uninitialized");
	}

	void PxeEngine::initSDL()
	{
		SDL_LogSetOutputFunction(sdlLogCallback, this);
		if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
			PXE_CHECKSDLERROR();
			PXE_FATAL("Failed to init SDL subsystems");
			return;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		PXE_CHECKSDLERROR();
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		PXE_CHECKSDLERROR();
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		PXE_CHECKSDLERROR();
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		PXE_CHECKSDLERROR();
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		PXE_CHECKSDLERROR();
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		PXE_CHECKSDLERROR();
		PXE_INFO("SDL initialized");
	}

	void PxeEngine::deinitSDL()
	{
		SDL_Quit();
		PXE_INFO("SDL uninitialized");
	}

	void PxeEngine::shutdown()
	{
		impl()._shouldShutdown = true;
	}

	PXE_NODISCARD bool PxeEngine::getShutdownPending() const
	{
		return impl()._shouldShutdown;
	}

	PXE_NODISCARD PxeLogInterface& PxeEngine::getLoggingInterface() const
	{
		return impl()._loggingInterface;
	}

	PXE_NODISCARD PxeInputManager& PxeEngine::getInputManager()
	{
		return impl()._inputManager;
	}

	PXE_NODISCARD const PxeInputManager& PxeEngine::getInputManager() const
	{
		return impl()._inputManager;
	}

	PXE_NODISCARD PxeRenderPipeline& PxeEngine::getRenderPipeline()
	{
		return impl()._renderPipeline;
	}

	PXE_NODISCARD const PxeRenderPipeline& PxeEngine::getRenderPipeline() const
	{
		return impl()._renderPipeline;
	}

	void PxeEngine::shutdownEngine()
	{
		PXE_INFO("PxeEngine shutdown");
		Impl& imp = impl();
		bindPrimaryGlContext();
		if (imp._initializedPrimaryGlContext)
			imp._renderPipeline.uninitializePipeline();
		// Clear the asset queue
		imp._assetMutex.lock();
		while (!imp._assetQueue.empty())
		{
			PxeGLAsset* asset = imp._assetQueue.front();
			imp._assetQueue.pop_front();
			imp._assetMutex.unlock();
			switch (asset->getAssetStatus())
			{
			case pxengine::PxeGLAssetStatus::PENDING_UNINITIALIZATION:
				asset->uninitialize();
				break;
			case pxengine::PxeGLAssetStatus::PENDING_INITIALIZATION:
				break;
			default:
				PXE_ERROR("PxeGlAsset queued not in pending state");
				break;
			}

			asset->drop();
			imp._assetMutex.lock();
		}

		imp._assetMutex.unlock();
		if (imp._primaryGlContext) {
			SDL_GL_DeleteContext(imp._primaryGlContext);
			PXE_CHECKSDLERROR();
			imp._primaryGlContext = nullptr;
		}

		if (imp._primarySDLWindow) {
			SDL_DestroyWindow(imp._primarySDLWindow);
			PXE_CHECKSDLERROR();
			imp._primarySDLWindow = nullptr;
		}

		if (getRefCount() > 1) {
			PXE_ERROR("PxeEngine still referenced, all references to the PxeEngine should be dropped before shutdown, this indicates a memory leak, still " + std::to_string(getRefCount() - 1) + " references");
		}
		else {
			PXE_INFO("PxeEngine shutdown complete");
		}

		drop();
	}

	PXE_NODISCARD PxeWindow* PxeEngine::createWindow(uint32_t width, uint32_t height, const char* title)
	{
		PxeWindow* window = new(std::nothrow) PxeWindow(width, height, title);
		if (!window) {
			PXE_ERROR("Failed to create window, failed to allocate PxeWindow");
			return nullptr;
		}

		window->_status = PxeGLAssetStatus::PENDING_INITIALIZATION;
		window->grab();
		Impl& imp = impl();
		std::lock_guard lock(imp._assetMutex);
		imp._assetQueue.emplace_front(window);
		imp._windowQueueForCreation = true;
		return window;
	}

	void PxeEngine::registerWindow(PxeWindow& window)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowsMutex);
		if (!imp._primaryGlContext) {
			imp._primaryGlContext = window.getGlContext();
			imp._primarySDLWindow = window.getSDLWindow();
		}

		imp._initializedWindows.emplace(window.getSDLWindowId(), &window);
	}

	void PxeEngine::unregisterWindow(PxeWindow& window)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowsMutex);
		imp._inputManager.clearActiveWindow(&window);
		if (!imp._initializedWindows.erase(window.getSDLWindowId())) {
			PXE_ERROR("Attempted to unregister unregistered PxeWindow");
		}

		if (imp._primaryGlContext == window.getGlContext()) {
			if (imp._initializedWindows.empty()) {
				if (!getShutdownPending()) {
					PXE_INFO("All PxeWindows destroyed, shutting down PxeEngine");
					shutdown();
				}

				window.setGlContext(nullptr);
				window.setSDLWindow(nullptr);
			}
			else {
				PxeWindow& newPrimaryWindow = *(imp._initializedWindows.begin()->second);
				window.bindGlContext();
				window.deleteFramebuffer();
				window.setGlContext(newPrimaryWindow.getGlContext());
				newPrimaryWindow.bindGlContext();
				newPrimaryWindow.deleteFramebuffer();
				newPrimaryWindow.setGlContext(imp._primaryGlContext);
				newPrimaryWindow.bindGlContext();
				newPrimaryWindow.createFramebuffer();
				imp._primarySDLWindow = newPrimaryWindow.getSDLWindow();
				PXE_INFO("Swapped primary PxeWindow");
			}
		}
	}

	PXE_NODISCARD PxeScene* PxeEngine::createScene(PxeSceneFlags flags)
	{
		Impl& imp = impl();
		physx::PxSceneDesc desc(imp._physScale);
		desc.gravity = physx::PxVec3(0, -9.81f, 0);
		desc.cpuDispatcher = imp._physDefaultDispatcher;
		desc.filterShader = physx::PxDefaultSimulationFilterShader;
		desc.flags |= physx::PxSceneFlag::eREQUIRE_RW_LOCK;
		physx::PxScene* physScene = imp._physPhysics->createScene(desc);
		if (!physScene) {
			PXE_ERROR("Failed to create PxeScene, failed to create PxeScene's physx::PxScene");
			return nullptr;
		}

		PxeScene* scene = new(std::nothrow) PxeScene(*physScene);
		if (!scene) {
			physScene->release();
			PXE_ERROR("Failed to create PxeScene, failed to allocate PxeScene");
			return nullptr;
		}

		scene->setFlags(flags);
		std::unique_lock lock(imp._sceneMutex);
		imp._scenes.emplace_back(scene);
		return scene;
	}

	void PxeEngine::removeScene(PxeScene& scene)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._sceneMutex);
		if (!imp._scenes.remove(&scene)) {
			PXE_ERROR("Attempted to remove unknown PxeScene");
		}
	}

	void PxeEngine::updateDeltaTime()
	{
		Impl& imp = impl();
		uint64_t ticks = SDL_GetTicks64();
		imp._lastDeltaTime = (float)(ticks - imp._lastDeltaUpdate);
		imp._lastDeltaUpdate = ticks;
	}

	void PxeEngine::initGlContext()
	{
		Impl& imp = impl();
		if (imp._initializedPrimaryGlContext) return;
		if (glewInit() != GLEW_OK) {
			PXE_FATAL("Failed to initialize GLEW");
		}

#ifdef PXE_DEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable(GL_DEBUG_OUTPUT);
		if (GLEW_VERSION_4_3) {
			glDebugMessageCallback(glErrorCallback, 0);
			PXE_INFO("Registered OpenGl error callback");
		}
#endif // PXE_DEBUG

		imp._renderPipeline.initializePipeline();
		imp._initializedPrimaryGlContext = true;
	}

	PXE_NODISCARD bool PxeEngine::createdPrimaryGlContext()
	{
		return impl()._primaryGlContext;
	}

	PXE_NODISCARD const std::unordered_map<uint32_t, PxeWindow*>& PxeEngine::acquireWindows()
	{
		Impl& imp = impl();
		imp._windowsMutex.lock_shared();
		return imp._initializedWindows;
	}

	void PxeEngine::releaseWindows()
	{
		impl()._windowsMutex.unlock_shared();
	}

	PXE_NODISCARD const std::list<PxeScene*>& PxeEngine::acquireScenes()
	{
		Impl& imp = impl();
		imp._sceneMutex.lock_shared();
		return imp._scenes;
	}

	void PxeEngine::releaseScenes()
	{
		impl()._sceneMutex.unlock_shared();
	}

	void PxeEngine::bindPrimaryGlContext()
	{
		Impl& imp = impl();
		if (imp._primaryGlContext && imp._primarySDLWindow)
			SDL_GL_MakeCurrent(imp._primarySDLWindow, imp._primaryGlContext);
	}

	void PxeEngine::initializeGlAsset(PxeGLAsset& asset)
	{
		if (asset.getAssetStatus() >= PxeGLAssetStatus::INITIALIZED) return;
		if (asset.getAssetStatus() != PxeGLAssetStatus::UNINITIALIZED) {
			PXE_ERROR("initializeAsset called on PxeGlAsset in a pending state");
			return;
		}

		asset.grab();
		asset._status = PxeGLAssetStatus::PENDING_INITIALIZATION;
		Impl& imp = impl();
		imp._assetMutex.lock();
		imp._assetQueue.emplace_back(&asset);
		imp._assetMutex.unlock();
	}

	void PxeEngine::forceInitializeGlAsset(PxeGLAsset& asset)
	{
		if (asset._delayedInitialization && asset.getAssetStatus() == PxeGLAssetStatus::UNINITIALIZED) {
			asset._status = PxeGLAssetStatus::PENDING_INITIALIZATION;
			asset.initialize();
		}
		else {
			{
				Impl& imp = impl();
				std::lock_guard lock(imp._assetMutex);
				if (asset.getAssetStatus() != PxeGLAssetStatus::PENDING_INITIALIZATION) {
					PXE_ERROR("Attempted to force asset initialization with asset thats status was not PENDING_INITIALIZATION");
					return;
				}

				auto itr = std::find(imp._assetQueue.begin(), imp._assetQueue.end(), &asset);
				if (itr == imp._assetQueue.end()) {
					PXE_ERROR("Asset was not queued for initialization but status was PENDING_INITIALIZATION");
					return;
				}

				imp._assetQueue.erase(itr);
			}

			asset.initialize();
			asset.drop();
		}
	}

	void PxeEngine::uninitializeGlAsset(PxeGLAsset& asset)
	{
		if (asset.getAssetStatus() == PxeGLAssetStatus::UNINITIALIZED) return;
		if (asset.getAssetStatus() < PxeGLAssetStatus::INITIALIZED) {
			PXE_ERROR("uninitializeGlAsset called on PxeGlAsset in a pending state");
			return;
		}

		asset.grab();
		asset._status = PxeGLAssetStatus::PENDING_UNINITIALIZATION;
		Impl& imp = impl();
		imp._assetMutex.lock();
		imp._assetQueue.emplace_back(&asset);
		imp._assetMutex.unlock();
	}

	void PxeEngine::updateAssetProcessing()
	{
		Impl& imp = impl();
		imp._assetMutex.lock();
		if (imp._windowQueueForCreation) {
			PxeGLAsset* asset = imp._assetQueue.front();
			imp._assetQueue.pop_front();
			imp._windowQueueForCreation = false;
			imp._assetMutex.unlock();
			asset->initialize();
			asset->drop();
			imp._assetMutex.lock();
		}

		if (createdPrimaryGlContext()) {
			bindPrimaryGlContext();
		}
		else {
			PXE_WARN("Attempted to update PxeGlAsset queue without PxeWindow");
			imp._assetMutex.unlock();
			return;
		}

		while (!imp._assetQueue.empty())
		{
			PxeGLAsset* asset = imp._assetQueue.front();
			imp._assetQueue.pop_front();
			imp._assetMutex.unlock();
			switch (asset->getAssetStatus())
			{
			case pxengine::PxeGLAssetStatus::PENDING_UNINITIALIZATION:
				asset->uninitialize();
				break;
			case pxengine::PxeGLAssetStatus::PENDING_INITIALIZATION:
				asset->initialize();
				break;
			default:
				PXE_ERROR("PxeGlAsset queued not in pending state");
				break;
			}

			asset->drop();
			imp._assetMutex.lock();
		}

		imp._assetMutex.unlock();
	}

	PXE_NODISCARD float PxeEngine::getDeltaTime() const
	{
		return impl()._lastDeltaTime;
	}

	PXE_NODISCARD physx::PxFoundation& PxeEngine::getPhysicsFoundation() const
	{
		return *impl()._physFoundation;
	}

	PXE_NODISCARD physx::PxPhysics& PxeEngine::getPhysicsBase() const
	{
		return *impl()._physPhysics;
	}

	PXE_NODISCARD physx::PxCooking& PxeEngine::getPhysicsCooking() const
	{
		return *impl()._physCooking;
	}

	void PxeEngine::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		PxeLogLevel level = PxeLogLevel::PXE_INFO;
		if (code | physx::PxErrorCode::eABORT) {
			level = PxeLogLevel::PXE_FATAL;
		}
		else if (code | physx::PxErrorCode::eINTERNAL_ERROR || code | physx::PxErrorCode::eOUT_OF_MEMORY || code | physx::PxErrorCode::eINVALID_OPERATION || code | physx::PxErrorCode::eINVALID_PARAMETER) {
			level = PxeLogLevel::PXE_ERROR;
		}
		else if (code | physx::PxErrorCode::ePERF_WARNING || code | physx::PxErrorCode::eDEBUG_WARNING) {
			level = PxeLogLevel::PXE_WARN;
		}

		impl()._loggingInterface.onLog(level, message, file, "physx::?", line);
	}

	void PxeEngine::operator()(const char* exp, const char* file, int line, bool& ignore)
	{
		pxeGetAssertInterface().onAssert(exp, file, "physx::?", line);
	}

	void GLAPIENTRY PxeEngine::glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
#ifdef PXE_DEBUG
		std::string messageStr = "GL Source: ";
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			messageStr += "GL_DEBUG_SOURCE_API";
			break;

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			messageStr += "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
			break;

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			messageStr += "GL_DEBUG_SOURCE_SHADER_COMPILER";
			break;

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			messageStr += "GL_DEBUG_SOURCE_THIRD_PARTY";
			break;

		case GL_DEBUG_SOURCE_APPLICATION:
			messageStr += "GL_DEBUG_SOURCE_APPLICATION";
			break;

		case GL_DEBUG_SOURCE_OTHER:
			messageStr += "GL_DEBUG_SOURCE_OTHER";
			break;

		default:
			messageStr += "UNKNOWN(" + std::to_string(source) + ")";
			break;
		}

		messageStr += " GL Type: ";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			messageStr += "GL_DEBUG_TYPE_ERROR";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			messageStr += "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			messageStr += "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			messageStr += "GL_DEBUG_TYPE_PORTABILITY";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			messageStr += "GL_DEBUG_TYPE_PERFORMANCE";
			break;

		case GL_DEBUG_TYPE_MARKER:
			messageStr += "GL_DEBUG_TYPE_MARKER";
			break;

		case GL_DEBUG_TYPE_PUSH_GROUP:
			messageStr += "GL_DEBUG_TYPE_PUSH_GROUP";
			break;

		case GL_DEBUG_TYPE_POP_GROUP:
			messageStr += "GL_DEBUG_TYPE_POP_GROUP";
			break;

		case GL_DEBUG_TYPE_OTHER:
			messageStr += "GL_DEBUG_TYPE_OTHER";
			break;

		default:
			messageStr += "UNKNOWN(" + std::to_string(type) + ")";
			break;
		}

		messageStr += " GL Id: " + std::to_string(id) + " GL Severity: ";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			messageStr += "GL_DEBUG_SEVERITY_HIGH";
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			messageStr += "GL_DEBUG_SEVERITY_MEDIUM";
			break;

		case GL_DEBUG_SEVERITY_LOW:
			messageStr += "GL_DEBUG_SEVERITY_LOW";
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			messageStr += "GL_DEBUG_SEVERITY_NOTIFICATION";
			break;

		default:
			messageStr += "UNKNOWN(" + std::to_string(type) + ")";
			break;
		}

		messageStr += " GL Message ";
		messageStr += message;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			PXE_ERROR(messageStr);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW:
			PXE_WARN(messageStr);
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
#ifndef PXE_DISABLE_GL_INFO_LOG
			PXE_INFO(messageStr);
#endif // !PXE_DISABLE_GL_INFO_LOG
			break;
		}
#endif // PXE_DEBUG
	}

	void PxeEngine::sdlLogCallback(void* userdata, int category, SDL_LogPriority priority, const char* message)
	{
		std::string str = "SDL: Category: " + std::to_string(category) + ", Message: " + message;
		switch (priority)
		{
		case SDL_LOG_PRIORITY_VERBOSE:
		case SDL_LOG_PRIORITY_DEBUG:
		case SDL_LOG_PRIORITY_INFO:
			PXE_INFO(str);
			break;

		case SDL_LOG_PRIORITY_WARN:
			PXE_WARN(str);
			break;

		case SDL_LOG_PRIORITY_ERROR:
			PXE_ERROR(str);
			break;

		case SDL_LOG_PRIORITY_CRITICAL:
			PXE_FATAL(str);
			break;
		}
	}
}