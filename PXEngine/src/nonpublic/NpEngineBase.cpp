#include "NpEngineBase.h"

#include <new>
#include <thread>
#include <algorithm>

#include "NpLogger.h"
#include "SDL.h"
#include "NpScene.h"

namespace pxengine {
	namespace nonpublic {
		NpEngineBase* NpEngineBase::s_instance = nullptr;

		NpEngineBase* NpEngineBase::createInstance(PxeLogInterface& logHandler)
		{
			if (s_instance) {
				PXE_WARN("Attempted to create PxeEngineBase when one already exist");
				return nullptr;
			}

			s_instance = new(std::nothrow) NpEngineBase(logHandler);
			PXE_ASSERT(s_instance, "Failed to allocate PxeEngineBase instance");
			PXE_INFO("Created PxeEngineBase instance");
			return s_instance;
		}

		NpEngineBase& NpEngineBase::getInstance()
		{
			PXE_ASSERT(s_instance, "Attempted to get invalid PxeEngineBase instance, PxeEngineBase not created");
			return *s_instance;
		}

		NpEngineBase::NpEngineBase(PxeLogInterface& logInterface) : _defaultPhysAssertHandler(physx::PxGetAssertHandler())
		{
			setLogInterface(logInterface);
			_engineInit = true;
			_sdlGlContext = nullptr;
			_activeWindow = nullptr;
			_activeKeyboardWindowId = 0;
			_activeMouseWindowId = 0;
			_boundContextThread = std::thread::id();
			if (std::thread::hardware_concurrency() <= 0) {
				PXE_WARN("Failed to get hardware thread count");
			}

			initSDL();
			initPhys();
		}

		NpEngineBase::~NpEngineBase()
		{
			if (_engineInit) {
				PXE_ERROR("PxeEngineBase not shutdown correctly, you must use the shutdown method");
			}

			deinitSDL();
			deinitPhys();
			s_instance = nullptr;
			PXE_INFO("PxeEngineBase Destroyed");
			clearLogInterface();
		}

		void NpEngineBase::acquireGlContextLock()
		{
			_glNextMutex.lock();
			_glMutex.lock();
			_glNextMutex.unlock();
		}

		void NpEngineBase::releaseGlContextLock()
		{
			_glMutex.unlock();
		}

		bool NpEngineBase::acquiredGlContext() const
		{
			return _boundContextThread == std::this_thread::get_id();
		}

		bool NpEngineBase::acquiredGlContext(const NpWindow& window) const
		{
			return (&window) == _activeWindow && acquiredGlContext();
		}

		void NpEngineBase::initPhys()
		{
			physx::PxSetAssertHandler(*this);
			_physScale.length = 1;
			_physScale.speed = 9.81;
			_physFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _physAllocator, *this);
			_physPVD = PxCreatePvd(*_physFoundation);
			_physPVDTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 1000);
			_physPVD->connect(*_physPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
			_physPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_physFoundation, _physScale, true, _physPVD);
			uint32_t threadCount = std::thread::hardware_concurrency() / 2;
			if (threadCount < 2) {
				PXE_WARN("Calculated physics thread count less then 2, using 2 threads");
				threadCount = 2;
			}
			// TODO Remove change how to detect if info logging enabled
#ifdef PXE_DEBUG
			else {
				char buf[64];
				sprintf_s(buf, "Using %d threads for default physics simulation job pool", threadCount);
				PXE_INFO(buf);
			}
#endif // PXE_DEBUG

			_physDefaultDispatcher = physx::PxDefaultCpuDispatcherCreate(threadCount);
			physx::PxCookingParams params(_physScale);
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			_physCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_physFoundation, params);
			PxInitExtensions(*_physPhysics, _physPVD);
		}

		void NpEngineBase::deinitPhys()
		{
			PxCloseExtensions();
			_physCooking->release();
			_physPhysics->release();
			_physPVD->disconnect();
			_physPVD->release();
			_physPVDTransport->release();
			_physFoundation->release();
			physx::PxSetAssertHandler(_defaultPhysAssertHandler);
		}

		void NpEngineBase::initSDL()
		{
			acquireGlContextLock();
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				PXE_FATAL("Failed to Init SDL");
				return;
			}

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

			// create a throwaway window used to create an OpenGl context
			// TODO find a better way to do this
			SDL_Window* tempWindow = SDL_CreateWindow("PXE_ENGINEBASE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS);
			if (!tempWindow) {
				PXE_CHECKSDLERROR();
				PXE_FATAL("Failed to Create SDL Window");
				return;
			}

			_sdlGlContext = SDL_GL_CreateContext(tempWindow);
			if (!_sdlGlContext) {
				PXE_CHECKSDLERROR();
				PXE_FATAL("Failed to Create OpenGl Context");
				return;
			}

			if (glewInit() != GLEW_OK) {
				PXE_FATAL("Failed to Init GLEW");
				return;
			}

			//glEnable(GL_DEBUG_OUTPUT);
			//glDebugMessageCallback(global::glErrorCallback, 0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glFrontFace(GL_CCW);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			SDL_GL_MakeCurrent(nullptr, nullptr);
			PXE_CHECKSDLERROR();
			SDL_DestroyWindow(tempWindow);
			PXE_CHECKSDLERROR();
			releaseGlContextLock();
		}

		void NpEngineBase::deinitSDL()
		{
			acquireGlContextLock();
			if (_sdlGlContext) {
				SDL_GL_DeleteContext(_sdlGlContext);
				PXE_CHECKSDLERROR();
				_sdlGlContext = nullptr;
			}

			SDL_Quit();
			releaseGlContextLock();
		}

		PxeWindow* NpEngineBase::createWindow(uint32_t width, uint32_t height, const char* title)
		{
			acquireGlContextLock();
			SDL_Window* sdlWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
			if (!sdlWindow) {
				PXE_ERROR("Failed to create SDL window");
				PXE_CHECKSDLERROR();
				releaseGlContextLock();
				return nullptr;
			}

			// Showing the window right away can create lockups if creating multiple widows at once
			// TODO Fix showing windows on creation
			//SDL_ShowWindow(sdlWindow);
			NpWindow* window = new(std::nothrow) NpWindow(*sdlWindow);
			if (!window) {
				PXE_ERROR("Failed to allocate PxeWindow");
				SDL_DestroyWindow(sdlWindow);
				PXE_CHECKSDLERROR();
				SDL_GL_MakeCurrent(nullptr, nullptr);
				PXE_CHECKSDLERROR();
				releaseGlContextLock();
				return nullptr;
			}

			_windows.emplace(SDL_GetWindowID(sdlWindow), window);
			window->setSwapInterval(1);
			SDL_GL_MakeCurrent(nullptr, nullptr);
			PXE_CHECKSDLERROR();
			PXE_INFO("Created window");
			releaseGlContextLock();
			return window;
		}

		PxeScene* NpEngineBase::createScene()
		{
			physx::PxSceneDesc desc(_physScale);
			desc.gravity = physx::PxVec3(0, -9.81f, 0);
			desc.cpuDispatcher = _physDefaultDispatcher;
			desc.filterShader = physx::PxDefaultSimulationFilterShader;
			physx::PxScene* physScene = _physPhysics->createScene(desc);
			if (!physScene) {
				PXE_ERROR("Failed to create PxeScene, failed to create physics scene");
				return nullptr;
			}

			NpScene* scene = new(std::nothrow) NpScene(physScene);
			if (!scene) {
				PXE_ERROR("Failed to create PxeScene, failed to allocate PxeScene");
				return nullptr;
			}

			return scene;
		}

		// TODO make this thread safe
		PxeShader* NpEngineBase::loadShader(const std::filesystem::path& path)
		{
			auto it = _shaders.find(path);
			if (it != _shaders.end()) {
				it->second->grab();
				return it->second;
			}
			else {
				NpShader* shader = new(std::nothrow) NpShader(path);
				if (shader) {
					_shaders.emplace(path, shader);
				}

				return shader;
			}
		}

		void NpEngineBase::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
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

		void NpEngineBase::operator()(const char* exp, const char* file, int line, bool& ignore)
		{
			pxeGetAssertInterface().onAssert(exp, file, "PHYSX(UNKNOWN)", line);
		}

		void NpEngineBase::acquireGlContext(NpWindow& window)
		{
			if (acquiredGlContext(window)) {
				if (_glContextDepth == 0xFF)
					PXE_ERROR("acquire depth counter overflow, attempted to acquire GlContext over 255 times");
				++_glContextDepth;
				return;
			}

			acquireGlContextLock();
			_activeWindow = &window;
			_boundContextThread = std::this_thread::get_id();
			SDL_GL_MakeCurrent(&(window.getSDLWindow()), _sdlGlContext);
			PXE_CHECKSDLERROR();
			SDL_GL_SetSwapInterval(window.getSwapInterval());
			PXE_CHECKSDLERROR();
			_glContextDepth = 1;
			updateQueuedAssets();
		}

		void NpEngineBase::releaseGlContext(NpWindow& window)
		{
			if (!acquiredGlContext(window)) {
				PXE_ERROR("Attempted to release GlContext not acquired by PxeWindow or acquired by a different thread");
			}
			else if (--_glContextDepth == 0) {
				_boundContextThread = std::thread::id();
				SDL_GL_MakeCurrent(nullptr, nullptr);
				PXE_CHECKSDLERROR();
				_activeWindow = nullptr;
				releaseGlContextLock();
			}
		}

		void NpEngineBase::pollEvents()
		{
			// TODO Manage all SDL events
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				// is it a quit request
				// this usually happens when the user request to close the last window
				// but there are other cases where this even can happen see below
				// https://wiki.libsdl.org/SDL2/SDL_EventType#remarks
				if (e.type == SDL_QUIT) {
					for (auto it = _windows.begin(); it != _windows.end(); ++it)
						it->second->setShouldClose();
				}
				// is it a window event
				else if (e.type == SDL_WINDOWEVENT)
				{
					SDL_WindowEventID winEvent = (SDL_WindowEventID)e.window.event;
					// check if we need to switch the active mouse or keyboard window id
					switch (winEvent)
					{
					case SDL_WINDOWEVENT_ENTER:
						_activeMouseWindowId = e.window.windowID;
						break;

					case SDL_WINDOWEVENT_LEAVE:
						if (_activeMouseWindowId == e.window.windowID)
							_activeMouseWindowId = 0;
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						_activeKeyboardWindowId = e.window.windowID;
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
						if (_activeKeyboardWindowId == e.window.windowID)
							_activeKeyboardWindowId = 0;
						break;
					}

					auto winIt = _windows.find(e.window.windowID);
					if (winIt != _windows.end()) {
						if (winEvent == SDL_WINDOWEVENT_CLOSE)
							winIt->second->setShouldClose();
						if (!winIt->second->getEventBuffer().insert(e)) {
							PXE_WARN("PxeWindow event buffer overflow");
						}
					}
				}
				// is it a keyboard input event
				else if (e.type >= SDL_KEYDOWN && e.type <= SDL_TEXTEDITING_EXT && e.type != SDL_KEYMAPCHANGED) {
					if (_activeKeyboardWindowId) {
						auto winIt = _windows.find(_activeKeyboardWindowId);
						if (winIt != _windows.end()) {
							if (!winIt->second->getEventBuffer().insert(e)) {
								PXE_WARN("PxeWindow event buffer overflow");
							}
						}
					}
				}
				// is it a mouse/joystick/touch input event
				else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MULTIGESTURE) {
					if (_activeMouseWindowId) {
						auto winIt = _windows.find(_activeMouseWindowId);
						if (winIt != _windows.end()) {
							if (!winIt->second->getEventBuffer().insert(e)) {
								PXE_WARN("PxeWindow event buffer overflow");
							}
						}
					}
				}
				// forward all other events to all windows
				else
				{
					for (auto it = _windows.begin(); it != _windows.end(); ++it) {
						if (!it->second->getEventBuffer().insert(e)) {
							PXE_WARN("PxeWindow event buffer overflow");
						}
					}
				}
			}
		}

		void NpEngineBase::updateQueuedAssets() {
			while (!_assetQueue.empty())
			{
				PxeGLAsset* asset = _assetQueue.front();
				_assetQueue.pop_front();
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
			}
		}

		void NpEngineBase::uninitializeGlAsset(PxeGLAsset& asset, bool blocking)
		{
			if (asset.getAssetStatus() == PxeGLAssetStatus::UNINITIALIZED) return;
			if (asset.getAssetStatus() < PxeGLAssetStatus::INITIALIZED) {
				PXE_ERROR("uninitializeGlAsset called on PxeGlAsset in a pending state");
				return;
			}

			asset._status = PxeGLAssetStatus::PENDING_UNINITIALIZATION;
			if (acquiredGlContext()) {
				asset.uninitialize();
			}
			else if (blocking) {
				// Hack as we need a window to bind the Gl context
				NpWindow& window = *(_windows.begin()->second);
				window.grab();
				acquireGlContext(window);
				asset.uninitialize();
				releaseGlContext(window);
				window.drop();
			}
			else {
				// TODO make this thread safe
				_assetQueue.emplace_back(&asset);
				asset.grab();
			}
		}

		void NpEngineBase::initializeGlAsset(PxeGLAsset& asset, bool blocking)
		{
			if (asset.getAssetStatus() >= PxeGLAssetStatus::INITIALIZED) return;
			if (asset.getAssetStatus() != PxeGLAssetStatus::UNINITIALIZED) {
				PXE_ERROR("initializeAsset called on PxeGlAsset in a pending state");
				return;
			}

			asset._status = PxeGLAssetStatus::PENDING_INITIALIZATION;
			if (acquiredGlContext()) {
				asset.initialize();
			}
			else if (blocking) {
				if (_windows.empty()) {
					PXE_ERROR("Failed to initialize PxeGlAsset, you must have at least one initialized window, asset not initialized");
					return;
				}

				// Hack as we need a window to bind the Gl context
				NpWindow& window = *(_windows.begin()->second);
				window.grab();
				acquireGlContext(window);
				asset.initialize();
				releaseGlContext(window);
				window.drop();
			}
			else {
				// TODO make this thread safe
				_assetQueue.emplace_back(&asset);
				asset.grab();
			}
		}

		bool NpEngineBase::getShutdown() const
		{
			return !_engineInit;
		}

		// TODO make this thread safe
		void NpEngineBase::removeShaderFromCache(const std::filesystem::path& path)
		{
#ifdef PXE_DEBUG
			PXE_INFO("Removed PxeShader " + path.string() + " from cache");
#endif // PXE_DEBUG

			_shaders.erase(path);
		}

		void NpEngineBase::shutdown()
		{
			_engineInit = false;
			if (getRefCount() > 1) {
				char buf[128];
				sprintf_s(buf, "PxeEngineBase still referenced, drop all references to the engine base before calling shutdown, still %llu references", getRefCount() - 1);
				PXE_ERROR(buf);
			}
			drop();
		}

		void NpEngineBase::destroyWindow(NpWindow& window)
		{
			if (_windows.size() == 1 && !_assetQueue.empty()) {
				acquireGlContext(window);
				releaseGlContext(window);
			}

			acquireGlContextLock();
			uint32_t windowId = window.getSDLWindowId();
			_windows.erase(windowId);
			if (_activeKeyboardWindowId == windowId)
				_activeKeyboardWindowId = 0;
			if (_activeMouseWindowId == windowId)
				_activeMouseWindowId = 0;
			SDL_DestroyWindow(&(window.getSDLWindow()));
			releaseGlContextLock();
		}
	}
}