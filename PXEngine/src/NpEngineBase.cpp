#include "NpEngineBase.h"

#include <new>
#include <thread>
#include <algorithm>

#include "NpLogger.h"
#include "SDL.h"
#include "NpScene.h"

#define PXE_ENGINEBASE_INFO(msg) onLog(msg, (uint8_t)pxengine::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_WARN(msg) onLog(msg, (uint8_t)pxengine::LogLevel::WARN, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_ERROR(msg) onLog(msg, (uint8_t)pxengine::LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_FATAL(msg) onLog(msg, (uint8_t)pxengine::LogLevel::FATAL, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_CHECKSDLERROR() do { const char* error = SDL_GetError(); if (*error != '\0') { PXE_ENGINEBASE_ERROR(error); SDL_ClearError(); } } while (0)

namespace pxengine {
	namespace nonpublic {
		NpEngineBase* NpEngineBase::s_instance = nullptr;

		NpEngineBase* NpEngineBase::createInstance(PxeLogHandler& logHandler)
		{
			if (s_instance) {
				PXE_WARN("PxeEngineBase Already Exist");
				return nullptr;
			}

			s_instance = new(std::nothrow) NpEngineBase(logHandler);
			PXE_ASSERT(s_instance, "Failed to allocate PxeEngineBase");
			PXE_INFO("PxeEngineBase Created");
			return s_instance;
		}

		NpEngineBase& NpEngineBase::getInstance()
		{
			PXE_ASSERT(s_instance, "PxeEngineBase Not Created");
			return *s_instance;
		}

		NpEngineBase::NpEngineBase(PxeLogHandler& logHandler) : _logHandler(logHandler), _defaultPhysAssertHandler(physx::PxGetAssertHandler())
		{
			_physInit = false;
			_sdlInit = false;
			_sdlGlContext = nullptr;
			_activeWindow = nullptr;
			_activeKeyboardWindowId = 0;
			_activeMouseWindowId = 0;
			_oldAssetCount = 0;
			_boundContextThread = std::thread::id();
			if (std::thread::hardware_concurrency() <= 0)
				PXE_ENGINEBASE_WARN("Failed to get hardware thread count");

			initSDL();
			initPhys();
		}

		void NpEngineBase::initPhys()
		{
			if (_physInit) {
				PXE_ENGINEBASE_WARN("Attempted to initialize already initialized Physics");
				return;
			}

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
				PXE_ENGINEBASE_WARN("Calculated physics thread count less then 2, using 2 threads");
				threadCount = 2;
			}
			else {
				char buf[64];
				sprintf_s(buf, "Using %d threads for default physics simulation job pool", threadCount);
				PXE_ENGINEBASE_INFO(buf);
			}

			_physDefaultDispatcher = physx::PxDefaultCpuDispatcherCreate(threadCount);
			physx::PxCookingParams params(_physScale);
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			_physCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_physFoundation, params);
			PxInitExtensions(*_physPhysics, _physPVD);
			_physInit = true;
		}

		void NpEngineBase::deinitPhys()
		{
			if (!_physInit) {
				PXE_ENGINEBASE_WARN("Attempted to deinitialize uninitialized Physics");
				return;
			}

			PxCloseExtensions();
			_physCooking->release();
			_physPhysics->release();
			_physPVD->disconnect();
			_physPVD->release();
			_physPVDTransport->release();
			_physFoundation->release();
			physx::PxSetAssertHandler(_defaultPhysAssertHandler);
			_physInit = false;
		}

		void NpEngineBase::initSDL()
		{
			if (_sdlInit) {
				PXE_ENGINEBASE_WARN("Attempted to initialize already initialized SDL");
				return;
			}

			if (SDL_Init(SDL_INIT_VIDEO) < 0)
				PXE_ENGINEBASE_FATAL("Failed to Init SDL");
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

			// create a throwaway window used to create an OpenGl context
			// TODO find a better way to do this
			SDL_Window* tempWindow = SDL_CreateWindow("PXE_ENGINEBASE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS);
			if (!tempWindow) {
				PXE_ENGINEBASE_CHECKSDLERROR();
				PXE_ENGINEBASE_FATAL("Failed to Create SDL Window");
			}

			_sdlGlContext = SDL_GL_CreateContext(tempWindow);
			if (!_sdlGlContext) {
				PXE_ENGINEBASE_CHECKSDLERROR();
				PXE_ENGINEBASE_FATAL("Failed to Create OpenGl Context");
			}

			if (glewInit() != GLEW_OK)
				PXE_ENGINEBASE_FATAL("Failed to Init GLEW");
			SDL_GL_MakeCurrent(nullptr, nullptr);
			PXE_ENGINEBASE_CHECKSDLERROR();
			SDL_DestroyWindow(tempWindow);
			PXE_ENGINEBASE_CHECKSDLERROR();
			_sdlInit = true;
		}

		void NpEngineBase::deinitSDL()
		{
			if (!_sdlInit) {
				PXE_ENGINEBASE_WARN("Attempted to deinitialize uninitialized SDL");
				return;
			}

			if (_sdlGlContext) {
				SDL_GL_DeleteContext(_sdlGlContext);
				PXE_ENGINEBASE_CHECKSDLERROR();
				_sdlGlContext = nullptr;
			}

			SDL_Quit();
			_sdlInit = false;
		}

		void NpEngineBase::uninitializeAssets()
		{
			PXE_ENGINEBASE_FATAL("Need to implement this");
		}

		NpEngineBase::~NpEngineBase()
		{
			if (!_initializedAssets.empty()) {
				PXE_ENGINEBASE_WARN("PxeGlAssets still initialized, uninitializing assets");
				uninitializeAssets();
			}

			deinitSDL();
			deinitPhys();
			s_instance = nullptr;
			PXE_ENGINEBASE_INFO("PxeEngineBase Destroyed");
		}

		PxeWindow* NpEngineBase::createWindow(uint32_t width, uint32_t height, const char* title)
		{
			_glWaitMutex.lock();
			std::lock_guard<std::recursive_mutex> lock(_glContextMutex);
			_glWaitMutex.unlock();
			SDL_Window* sdlWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
			if (!sdlWindow) {
				PXE_ENGINEBASE_ERROR("Failed to Create SDL Window");
				PXE_ENGINEBASE_CHECKSDLERROR();
				return nullptr;
			}

			//SDL_ShowWindow(sdlWindow);
			NpWindow* window = new(std::nothrow) NpWindow(*sdlWindow);
			if (!window) {
				PXE_ENGINEBASE_ERROR("Failed to Allocate Window");
				SDL_DestroyWindow(sdlWindow);
				PXE_ENGINEBASE_CHECKSDLERROR();
				return nullptr;
			}

			_eventWindows.emplace(SDL_GetWindowID(sdlWindow), window);
			window->setSwapInterval(1);
			SDL_GL_MakeCurrent(nullptr, nullptr);
			PXE_ENGINEBASE_CHECKSDLERROR();
			PXE_ENGINEBASE_INFO("Window Created");
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
				PXE_ENGINEBASE_ERROR("Failed to create physics scene");
				return nullptr;
			}

			NpScene* scene = new(std::nothrow) NpScene(physScene);
			if (!scene) {
				PXE_ENGINEBASE_ERROR("Failed to Allocate Scene");
				return nullptr;
			}

			return scene;
		}

		void NpEngineBase::onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function)
		{
			_logHandler.onLog(msg, level, file, line, function);
		}

		void NpEngineBase::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
		{
			onLog(message, code, file, line, "UNKNOWN");
		}

		void NpEngineBase::operator()(const char* exp, const char* file, int line, bool& ignore)
		{
			getPXEAssertHandler().onAssert(exp, file, line, "UNKNOWN");
		}

		void NpEngineBase::acquireGlContext(NpWindow& window)
		{
			_glWaitMutex.lock();
			_glContextMutex.lock();
			_glWaitMutex.unlock();
			_activeWindow = &window;
			_boundContextThread = std::this_thread::get_id();
			SDL_GL_MakeCurrent(&(window.getSDLWindow()), _sdlGlContext);
			PXE_ENGINEBASE_CHECKSDLERROR();
			SDL_GL_SetSwapInterval(window.getSwapInterval());
			PXE_ENGINEBASE_CHECKSDLERROR();
			while (_oldAssetCount > 0)
			{
				--_oldAssetCount;
				PxeGLAsset* asset = _initializedAssets.back();
				_initializedAssets.pop_back();
				asset->uninitialize();
			}
		}

		void NpEngineBase::releaseGlContext(NpWindow& window)
		{
			if ((&window) != _activeWindow) {
				PXE_ENGINEBASE_ERROR("Attempted to release GlContext not acquired by window");
				return;
			}

			_boundContextThread = std::thread::id();
			SDL_GL_MakeCurrent(nullptr, nullptr);
			_activeWindow = nullptr;
			_glContextMutex.unlock();
		}

		void NpEngineBase::pollEvents()
		{
			//char buf[512] = "";
			//sprintf_s(buf, "Window: %i, Event: %i", e.window.windowID, e.window.event);
			//PXE_INFO(buf);
			// TODO manage other events

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				// is it a window event
				if (e.type == SDL_WINDOWEVENT)
				{
					char buf[128];
					SDL_WindowEventID winEvent = (SDL_WindowEventID)e.window.event;
					// check if we need to switch the active mouse or keyboard window
					switch (winEvent)
					{
					case SDL_WINDOWEVENT_ENTER:
						sprintf_s(buf, "MOUSE ENTER Old Window: %i, New Window: %i", _activeMouseWindowId, e.window.windowID);
						_activeMouseWindowId = e.window.windowID;
						PXE_ENGINEBASE_INFO(buf);
						break;

					case SDL_WINDOWEVENT_LEAVE:
					{
						uint32_t oldMouse = _activeMouseWindowId;
						if (_activeMouseWindowId == e.window.windowID)
							_activeMouseWindowId = 0;
						sprintf_s(buf, "MOUSE LEAVE Old Window: %i, New Window: %i", oldMouse, _activeMouseWindowId);
						PXE_ENGINEBASE_INFO(buf);
					}
					break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						sprintf_s(buf, "KEYBOARD ENTER Old Window: %i, New Window: %i", _activeKeyboardWindowId, e.window.windowID);
						_activeKeyboardWindowId = e.window.windowID;
						PXE_ENGINEBASE_INFO(buf);
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
					{
						uint32_t oldKeyboard = _activeKeyboardWindowId;
						if (_activeKeyboardWindowId == e.window.windowID)
							_activeKeyboardWindowId = 0;
						sprintf_s(buf, "KEYBOARD LEAVE Old Window: %i, New Window: %i", oldKeyboard, _activeKeyboardWindowId);
						PXE_ENGINEBASE_INFO(buf);
					}
					break;
					}

					auto winIt = _eventWindows.find(e.window.windowID);
					if (winIt != _eventWindows.end()) {
						if (!winIt->second->getEventBuffer().insert(e)) {
							PXE_ENGINEBASE_WARN("Window event buffer overflow");
						}
					}
				}
				// is it a keyboard input event
				else if (e.type >= SDL_KEYDOWN && e.type <= SDL_TEXTEDITING_EXT && e.type != SDL_KEYMAPCHANGED) {
					if (_activeKeyboardWindowId) {
						auto winIt = _eventWindows.find(_activeKeyboardWindowId);
						if (winIt != _eventWindows.end()) {
							if (!winIt->second->getEventBuffer().insert(e)) {
								PXE_ENGINEBASE_WARN("Window event buffer overflow");
							}
						}
					}
				}
				// is it a mouse/joystick/touch input event
				else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MULTIGESTURE) {
					if (_activeMouseWindowId) {
						auto winIt = _eventWindows.find(_activeMouseWindowId);
						if (winIt != _eventWindows.end()) {
							if (!winIt->second->getEventBuffer().insert(e)) {
								PXE_ENGINEBASE_WARN("Window event buffer overflow");
							}
						}
					}
				}
				// forward all other events to all windows
				else
				{
					for (auto it = _eventWindows.begin(); it != _eventWindows.end(); ++it) {
						if (!it->second->getEventBuffer().insert(e)) {
							PXE_ENGINEBASE_WARN("Window event buffer overflow");
						}
					}
				}
			}
		}

		bool NpEngineBase::uninitializeGlAsset(PxeGLAsset& asset, bool blocking)
		{
			if (!asset.getInitialized()) {
				PXE_ENGINEBASE_WARN("attempted to uninitialize uninitialized PxeGlAsset");
				return true;
			}

			if (_boundContextThread == std::this_thread::get_id()) {
				asset.uninitialize();
				_initializedAssets.remove(&asset);
				return true;
			}
			else if (blocking) {
				// Hack as we need a window to bind the Gl context
				NpWindow& window = *(_eventWindows.begin()->second);
				window.grab();
				acquireGlContext(window);
				asset.uninitialize();
				_initializedAssets.remove(&asset);
				releaseGlContext(window);
				window.drop();
				return true;
			}
			else {
				// TODO make this thread safe
				auto it = std::find(_initializedAssets.begin(), _initializedAssets.end(), &asset);
				if (it == _initializedAssets.end()) {
					PXE_ENGINEBASE_ERROR("Failed to find GlAsset");
					return true;
				}

				_initializedAssets.splice(_initializedAssets.end(), _initializedAssets, it);
				_oldAssetCount += 1;
				asset.grab();
			}

			return false;
		}

		void NpEngineBase::initializeGlAsset(PxeGLAsset& asset)
		{
			if (_boundContextThread == std::this_thread::get_id()) {
				asset.initialize();
				_initializedAssets.emplace_front(&asset);
			}
			else {
				// Hack as we need a window to bind the Gl context
				NpWindow& window = *(_eventWindows.begin()->second);
				window.grab();
				acquireGlContext(window);
				asset.initialize();
				_initializedAssets.emplace_front(&asset);
				releaseGlContext(window);
				window.drop();
			}
		}

		void NpEngineBase::removeWindowFromEventQueue(NpWindow& window)
		{
			_glContextMutex.lock();
			uint32_t windowId = SDL_GetWindowID(&(window.getSDLWindow()));
			_eventWindows.erase(windowId);
			if (_activeKeyboardWindowId == windowId)
				_activeKeyboardWindowId = 0;
			if (_activeMouseWindowId == windowId)
				_activeMouseWindowId = 0;
			_glContextMutex.unlock();
		}

		void NpEngineBase::onDelete()
		{

		}
	}
}