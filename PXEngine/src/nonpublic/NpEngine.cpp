#include "NpEngine.h"

#include <thread>
#include <string>
#include <new>

#include "NpLogger.h"
#include "PxPhysicsVersion.h"
#include "extensions/PxExtensionsAPI.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl.h"
#include "PxeRenderTexture.h"

namespace pxengine {
	namespace nonpublic {
		NpEngine* NpEngine::s_instance = nullptr;

		PXE_NODISCARD NpEngine& NpEngine::getInstance()
		{
			PXE_ASSERT(s_instance, "Attempted to get invalid PxeEngine instance");
			return *s_instance;
		}

		NpEngine::NpEngine(PxeLogInterface& logInterface) : _defaultPhysAssertHandler(physx::PxGetAssertHandler()), _logInterface(logInterface)
		{
			PXE_ASSERT(!s_instance, "Attempted to create new PxeEngine when one exist");
			s_instance = this; // all logging function work from this point on
			PXE_INFO("PxeEngine Created");
			_primaryGlContext = nullptr;
			_primarySDLWindow = nullptr;
			_renderPipeline = nullptr;
			_inputManager = nullptr;
			_shutdownFlag = false;
			_windowQueueForCreation = false;
			_renderInit = false;

			_inputManager = new(std::nothrow) NpInputManager();
			if (!_inputManager) {
				PXE_FATAL("Failed to create PxeInputManager for PxeEngine");
			}

			_renderPipeline = new(std::nothrow) NpRenderPipeline();
			if (!_renderPipeline) {
				PXE_FATAL("Failed to create PxeRenderPipeline for PxeEngine");
			}

			initSDL();
			initPhysics();
			PXE_INFO("PxeEngine ready");
		}

		NpEngine::~NpEngine()
		{
			deinitSDL();
			deinitPhysics();
			delete _inputManager;
			PXE_INFO("PxeEngine destroyed");
			s_instance = nullptr; //no logging functions work from this point on
		}

		void GLAPIENTRY NpEngine::glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam)
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

		void NpEngine::sdlLogOutput(void* userdata, int category, SDL_LogPriority priority, const char* message)
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

		void NpEngine::initPhysics()
		{
			if (std::thread::hardware_concurrency() <= 0) {
				PXE_WARN("Failed to get hardware thread count");
			}

			physx::PxSetAssertHandler(*this);
			_physScale.length = 1;
			_physScale.speed = 9.81f;
			_physFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _physAllocator, *this);
			if (!_physFoundation) {
				PXE_FATAL("Failed to create Physics Foundation for PxeEngine");
			}

#ifdef PXE_DEBUG
			_physPVD = PxCreatePvd(*_physFoundation);
			if (!_physPVD) {
				PXE_FATAL("Failed to create Physics Debugging Interface for PxeEngine");
			}

			_physPVDTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 1000);
			if (!_physPVDTransport) {
				PXE_FATAL("Failed to create Physics Debugging Transport Interface for PxeEngine");
			}

			_physPVD->connect(*_physPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
#define PXE_PHYSICS_PVD_POINTER _physPVD
#define PXE_PHYSICS_TRACK_ALLOCATIONS true
#else
#define PXE_PHYSICS_PVD_POINTER nullptr
#define PXE_PHYSICS_TRACK_ALLOCATIONS false
#endif
			_physPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *_physFoundation, _physScale, PXE_PHYSICS_TRACK_ALLOCATIONS, PXE_PHYSICS_PVD_POINTER);
			if (!_physPhysics) {
				PXE_FATAL("Failed to create Physics Base for PxeEngine");
			}

			uint32_t threadCount = std::thread::hardware_concurrency() / 2;
			if (threadCount < 2) {
				PXE_WARN("Calculated physics thread count less then 2, using 2 threads");
				threadCount = 2;
			}
			else {
				PXE_INFO("Using " + std::to_string(threadCount) + " threads for default physics simulation job pool");
			}

			_physDefaultDispatcher = physx::PxDefaultCpuDispatcherCreate(threadCount);
			if (!_physDefaultDispatcher) {
				PXE_FATAL("Failed to create Physics Dispatcher for PxeEngine");
			}

			physx::PxCookingParams params(_physScale);
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			_physCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_physFoundation, params);
			if (!_physCooking) {
				PXE_FATAL("Failed to create Physics Cooking Interface for PxeEngine");
			}

			if (!PxInitExtensions(*_physPhysics, PXE_PHYSICS_PVD_POINTER)) {
				PXE_FATAL("Failed to init Physics Extensions");
			}
		}

		void NpEngine::deinitPhysics()
		{
			PxCloseExtensions();
			_physDefaultDispatcher->release();
			_physCooking->release();
			_physPhysics->release();
#ifdef PXE_DEBUG
			_physPVD->disconnect();
			_physPVD->release();
			_physPVDTransport->release();
#endif
			_physFoundation->release();
			physx::PxSetAssertHandler(_defaultPhysAssertHandler);
		}

		void NpEngine::initSDL()
		{
			SDL_LogSetOutputFunction(sdlLogOutput, this);
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
		}

		void NpEngine::deinitSDL()
		{
			SDL_Quit();
		}

		PXE_NODISCARD PxeWindow* NpEngine::createWindow(uint32_t width, uint32_t height, const char* title)
		{
			NpWindow* window = new(std::nothrow) NpWindow(width, height, title);
			if (!window) {
				PXE_ERROR("Failed to allocate PxeWindow");
				return nullptr;
			}

			window->_status = PxeGLAssetStatus::PENDING_INITIALIZATION;
			window->grab();
			std::lock_guard lock(_assetMutex);
			_assetQueue.emplace_front(window);
			_windowQueueForCreation = true;
			return window;
		}

		void NpEngine::removeScene(NpScene& scene)
		{
			std::unique_lock lock(_sceneMutex);
			if (!_scenes.remove(&scene)) {
				PXE_ERROR("Attempted to remove PxeScene that was not inserted to scene list");
			}
		}

		PXE_NODISCARD PxeLogInterface& NpEngine::getLogInterface()
		{
			return _logInterface;
		}

		PXE_NODISCARD NpInputManager& NpEngine::getNpInputManager() const
		{
			return *_inputManager;
		}

		PXE_NODISCARD NpRenderPipeline& NpEngine::getNpRenderPipeline() const
		{
			return *_renderPipeline;
		}

		PXE_NODISCARD bool NpEngine::hasPrimaryGlContext() const
		{
			return _primaryGlContext;
		}

		void NpEngine::initGlContext() {
			if (_renderInit) return;
			if (glewInit() != GLEW_OK) {
				PXE_FATAL("Failed to Init GLEW");
				return;
			}

#ifdef PXE_DEBUG
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glEnable(GL_DEBUG_OUTPUT);
			if (GLEW_VERSION_4_3) {
				glDebugMessageCallback(glErrorCallback, 0);
				PXE_INFO("Registered OpenGl error callback");
			}
#endif // PXE_DEBUG
			_renderPipeline->initializePipeline();
			_renderInit = true;
		}

		void NpEngine::forceInitializeGlAsset(PxeGLAsset& asset)
		{
			bindPrimaryGlContext();
			if (asset._delayedInitialization && asset.getAssetStatus() == PxeGLAssetStatus::UNINITIALIZED) {
				asset._status = PxeGLAssetStatus::PENDING_INITIALIZATION;
				asset.initialize();
			}
			else {
				std::lock_guard lock(_assetMutex);
				if (asset.getAssetStatus() != PxeGLAssetStatus::PENDING_INITIALIZATION) {
					PXE_WARN("Attempted to force asset initialization with asset thats status was not PENDING_INITIALIZATION");
					return;
				}

				asset.initialize();
				bool removed = _assetQueue.remove(&asset);
				if (removed) {
					asset.drop();
				}
				else {
					PXE_ERROR("Asset was not queued for initialization but status was PENDING_INITIALIZATION");
				}
			}
		}

		PxeScene* NpEngine::createScene()
		{
			physx::PxSceneDesc desc(_physScale);
			desc.gravity = physx::PxVec3(0, -9.81f, 0);
			desc.cpuDispatcher = _physDefaultDispatcher;
			desc.filterShader = physx::PxDefaultSimulationFilterShader;
			desc.flags |= physx::PxSceneFlag::eREQUIRE_RW_LOCK;
			physx::PxScene* physScene = _physPhysics->createScene(desc);
			if (!physScene) {
				PXE_ERROR("Failed to create PxeScene's physics scene");
				return nullptr;
			}

			NpScene* scene = new(std::nothrow) NpScene(physScene);
			if (!scene) {
				PXE_ERROR("Failed to allocate PxeScene");
				return nullptr;
			}

			std::unique_lock lock(_sceneMutex);
			_scenes.emplace_back(scene);
			return static_cast<PxeScene*>(scene);
		}

		PXE_NODISCARD PxeInputManager& NpEngine::getInputManager() const
		{
			return static_cast<PxeInputManager&>(*_inputManager);
		}

		PXE_NODISCARD float NpEngine::getDeltaTime() const
		{
			return _deltaTime;
		}

		PXE_NODISCARD physx::PxFoundation& NpEngine::getPhysicsFoundation() const
		{
			return *_physFoundation;
		}

		PXE_NODISCARD physx::PxPhysics& NpEngine::getPhysicsBase() const
		{
			return *_physPhysics;
		}

		PXE_NODISCARD physx::PxCooking& NpEngine::getPhysicsCooking() const
		{
			return *_physCooking;
		}

		PXE_NODISCARD PxeRenderPipeline& NpEngine::getRenderPipeline() const
		{
			return static_cast<PxeRenderPipeline&>(*_renderPipeline);
		}

		void NpEngine::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
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

			log(level, message, file, "PHYSX(UNKNOWN)", line);
		}

		void NpEngine::operator()(const char* exp, const char* file, int line, bool& ignore)
		{
			pxeGetAssertInterface().onAssert(exp, file, "PHYSX(UNKNOWN)", line);
		}

		void NpEngine::processAssetQueue() {
			_assetMutex.lock();
			if (_windowQueueForCreation) {
				PxeGLAsset* asset = _assetQueue.front();
				_assetQueue.pop_front();
				asset->initialize();
				_windowQueueForCreation = false;
				_assetMutex.unlock();
				asset->drop();
				_assetMutex.lock();
			}

			if (hasPrimaryGlContext()) {
				bindPrimaryGlContext();
			}
			else {
				PXE_WARN("Attempted to update PxeGlAsset queue without PxeWindow");
				_assetMutex.unlock();
				return;
			}

			while (!_assetQueue.empty())
			{
				PxeGLAsset* asset = _assetQueue.front();
				_assetQueue.pop_front();
				_assetMutex.unlock();
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
				_assetMutex.lock();
			}

			_assetMutex.unlock();
		}

		void NpEngine::uninitializeGlAsset(PxeGLAsset& asset)
		{
			if (asset.getAssetStatus() == PxeGLAssetStatus::UNINITIALIZED) return;
			if (asset.getAssetStatus() < PxeGLAssetStatus::INITIALIZED) {
				PXE_ERROR("uninitializeGlAsset called on PxeGlAsset in a pending state");
				return;
			}

			asset._status = PxeGLAssetStatus::PENDING_UNINITIALIZATION;
			_assetMutex.lock();
			_assetQueue.emplace_back(&asset);
			_assetMutex.unlock();
			asset.grab();
		}

		void NpEngine::registerWindow(NpWindow& window)
		{
			std::unique_lock lock(_windowsMutex);
			if (!hasPrimaryGlContext()) {
				_primaryGlContext = window._sdlGlContext;
				_primarySDLWindow = window._sdlWindow;
			}

			_sdlWindows.emplace(window.getSDLWindowId(), &window);
		}

		void NpEngine::unregisterWindow(NpWindow& window)
		{
			std::unique_lock lock(_windowsMutex);
			_inputManager->clearActiveWindow(&window);
			if (!_sdlWindows.erase(window.getSDLWindowId())) {
				PXE_ERROR("Attempted to remove unregistered PxeWindow from window list");
			}

			if (_primaryGlContext == window._sdlGlContext) {
				if (_sdlWindows.empty()) {
					if (!getShutdownFlag()) {
						PXE_INFO("All windows destroyed, shutting down engine");
						shutdown();
					}

					window._sdlGlContext = nullptr;
					window._sdlWindow = nullptr;
				}
				else {
					NpWindow& newPrimaryWindow = *(_sdlWindows.begin()->second);
					window.bindGlContext();
					window.deleteFramebuffer();
					window._sdlGlContext = newPrimaryWindow._sdlGlContext;
					newPrimaryWindow.bindGlContext();
					newPrimaryWindow.deleteFramebuffer();
					newPrimaryWindow._sdlGlContext = _primaryGlContext;
					newPrimaryWindow.bindGlContext();
					newPrimaryWindow.createFramebuffer();
					_primarySDLWindow = newPrimaryWindow._sdlWindow;
					PXE_INFO("Swapped primary window");
				}
			}
		}

		PXE_NODISCARD bool NpEngine::getShutdownFlag() const
		{
			return _shutdownFlag;
		}

		void NpEngine::initializeGlAsset(PxeGLAsset& asset)
		{
			if (asset.getAssetStatus() >= PxeGLAssetStatus::INITIALIZED) return;
			if (asset.getAssetStatus() != PxeGLAssetStatus::UNINITIALIZED) {
				PXE_ERROR("initializeAsset called on PxeGlAsset in a pending state");
				return;
			}

			asset._status = PxeGLAssetStatus::PENDING_INITIALIZATION;
			_assetMutex.lock();
			_assetQueue.emplace_back(&asset);
			_assetMutex.unlock();
			asset.grab();
		}

		void NpEngine::shutdown() {
			_shutdownFlag = true;
		}

		void NpEngine::shutdownEngine()
		{
			PXE_INFO("PxeEngine shutdown");
			bindPrimaryGlContext();
			if (_renderInit)
				_renderPipeline->uninitializePipeline();
			// Clear the asset queue
			_assetMutex.lock();
			while (!_assetQueue.empty())
			{
				PxeGLAsset* asset = _assetQueue.front();
				_assetQueue.pop_front();
				_assetMutex.unlock();
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
				_assetMutex.lock();
			}

			_assetMutex.unlock();
			if (!_renderPipeline->drop()) {
				PXE_ERROR("PxeRenderPipeline not dropped, all references to the PxeRenderPipeline should be dropped before shutdown, this indicates a memory leak, still " + std::to_string(_renderPipeline->getRefCount()) + " references");
			}

			_renderPipeline = nullptr;

			if (_primaryGlContext) {
				SDL_GL_DeleteContext(_primaryGlContext);
				PXE_CHECKSDLERROR();
				_primaryGlContext = nullptr;
			}

			if (_primarySDLWindow) {
				SDL_DestroyWindow(_primarySDLWindow);
				PXE_CHECKSDLERROR();
				_primarySDLWindow = nullptr;
			}

			if (getRefCount() > 1) {
				PXE_ERROR("PxeEngine still referenced, all references to the PxeEngine should be dropped before shutdown, this indicates a memory leak, still " + std::to_string(getRefCount() - 1) + " references");
			}
			else {
				PXE_INFO("PxeEngine shutdown complete");
			}

			drop();
		}

		const std::unordered_map<uint32_t, NpWindow*>& NpEngine::getWindows()
		{
			return _sdlWindows;
		}

		const std::list<NpScene*>& NpEngine::getScenes()
		{
			return _scenes;
		}

		//void NpEngine::newFrame(NpWindow& window)
		//{
		//	bindPrimaryGlContext();
		//	window.updateSDLWindow();
		//	window.acquireReadLock();
		//	if (window.getPrimaryWindow()) {
		//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//	}
		//	else {
		//		window._renderTexture->bind();
		//		window._renderTexture->resize(window.getWindowWidth(), window.getWindowHeight());
		//	}

		//	glViewport(0, 0, window.getWindowWidth(), window.getWindowHeight());
		//	glClearColor(0, 0, 0, 1.0f);
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//	window.bindGuiContext();
		//	ImGuiIO& io = ImGui::GetIO();
		//	if (_inputManager->getMouseFocusedWindow() == &window) {
		//		io.ConfigFlags &= !ImGuiConfigFlags_NoMouseCursorChange;
		//	}
		//	else {
		//		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		//	}

		//	ImGui_ImplSDL2_NewFrame();
		//	ImGui::NewFrame();
		//	window.releaseReadLock();
		//}

		//void NpEngine::renderFrame(NpWindow& window)
		//{
		//	bindPrimaryGlContext();
		//	window.acquireReadLock();
		//	NpScene* scene = window.getNpScene();
		//	if (scene) scene->grab();
		//	window.releaseReadLock();
		//	if (!scene) return;
		//	scene->acquireObjectsReadLock();
		//	const std::list<PxeGeometryObjectInterface*>& renderList = scene->getGeometryObjectList(PxeRenderPass::FORWARD);
		//	if (renderList.empty()) {
		//		scene->releaseObjectsReadLock();
		//		scene->drop();
		//		return;
		//	}

		//	PxeShader* activeShader = nullptr;
		//	PxeRenderMaterial* activeMaterial = nullptr;
		//	int32_t mvpLocation = -1;
		//	PxeCamera& camera = *window.getCamera();
		//	camera.setWindowSize(window.getWindowWidth(), window.getWindowHeight());
		//	glm::mat4 pvMatrix(camera.getPVMatrix());
		//	for (PxeGeometryObjectInterface* geo : renderList) {
		//		if (&(geo->getRenderMaterial()) != activeMaterial) {
		//			activeMaterial = &(geo->getRenderMaterial());
		//			if (&(activeMaterial->getShader()) != activeShader) {
		//				activeShader = &(activeMaterial->getShader());
		//				activeShader->bind();
		//				mvpLocation = activeShader->getUniformLocation("u_MVP");
		//			}

		//			activeMaterial->applyMaterial();
		//		}

		//		activeShader->setUniformM4f(mvpLocation, pvMatrix * geo->getPositionMatrix());
		//		geo->onGeometry();
		//	}

		//	if (activeShader) activeShader->unbind();
		//	scene->releaseObjectsReadLock();
		//	scene->drop();
		//}

		//void NpEngine::renderGui(NpWindow& window)
		//{
		//	window.acquireReadLock();
		//	window.bindGuiContext();
		//	NpScene* scene = window.getNpScene();
		//	if (scene) scene->grab();
		//	window.releaseReadLock();
		//	if (!scene) return;
		//	scene->acquireObjectsReadLock();
		//	const std::list<PxeGUIObjectInterface*>& renderList = scene->getGUIObjectList();
		//	if (renderList.empty()) {
		//		scene->releaseObjectsReadLock();
		//		scene->drop();
		//		return;
		//	}

		//	for (PxeGUIObjectInterface* renderObj : renderList)
		//		renderObj->onGUI();
		//	scene->releaseObjectsReadLock();
		//	scene->drop();
		//}

		void NpEngine::bindPrimaryGlContext()
		{
			if (_primarySDLWindow && _primaryGlContext)
				SDL_GL_MakeCurrent(_primarySDLWindow, _primaryGlContext);
		}

		void NpEngine::acquireWindowsReadLock()
		{
			_windowsMutex.lock_shared();
		}

		void NpEngine::releaseWindowsReadLock()
		{
			_windowsMutex.unlock_shared();
		}

		void NpEngine::acquireScenesReadLock()
		{
			_sceneMutex.lock_shared();
		}

		void NpEngine::releaseScenesReadLock()
		{
			_sceneMutex.unlock_shared();
		}

		void NpEngine::setDeltaTime(float dt)
		{
			_deltaTime = dt;
		}
	}
}