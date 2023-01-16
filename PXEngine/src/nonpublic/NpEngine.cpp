#include "NpEngine.h"

#include <thread>
#include <string>
#include <new>

#include "NpLogger.h"
#include "PxPhysicsVersion.h"
#include "extensions/PxExtensionsAPI.h"
#include "SDL.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderObject.h"

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
			_sdlGlContext = nullptr;
			_activeKeyboardWindowId = 0;
			_activeMouseWindowId = 0;
			_primaryWindowId = PxeInvalidWindowId;
			_shutdownFlag = 0;

			initSDL();
			initPhysics();
			PXE_INFO("PxeEngine Ready");
		}

		NpEngine::~NpEngine()
		{
			if (_sdlGlContext)
				deinitRendering();
			deinitSDL();
			deinitPhysics();
			PXE_INFO("PxeEngine Destroyed");
			s_instance = nullptr; //no logging function work from this point on
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
				PXE_INFO(messageStr);
				break;
			}
#endif // PXE_DEBUG
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
#ifdef PXE_DEBUG
			_physPVD = PxCreatePvd(*_physFoundation);
			_physPVDTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 1000);
			_physPVD->connect(*_physPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
#define PXE_PHYSICS_PVD_POINTER _physPVD
#else
#define PXE_PHYSICS_PVD_POINTER nullptr
#endif
			_physPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_physFoundation, _physScale, true, PXE_PHYSICS_PVD_POINTER);
			uint32_t threadCount = std::thread::hardware_concurrency() / 2;
			if (threadCount < 2) {
				PXE_WARN("Calculated physics thread count less then 2, using 2 threads");
				threadCount = 2;
			}
			else {
				PXE_INFO("Using " + std::to_string(threadCount) + " threads for default physics simulation job pool");
			}

			_physDefaultDispatcher = physx::PxDefaultCpuDispatcherCreate(threadCount);
			physx::PxCookingParams params(_physScale);
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
			params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
			_physCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_physFoundation, params);
			PxInitExtensions(*_physPhysics, PXE_PHYSICS_PVD_POINTER);
		}

		void NpEngine::deinitPhysics()
		{
			PxCloseExtensions();
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
			if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
				PXE_CHECKSDLERROR();
				PXE_FATAL("Failed to init SDL subsystems");
				return;
			}

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			PXE_CHECKSDLERROR();
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			PXE_CHECKSDLERROR();
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			PXE_CHECKSDLERROR();
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
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

		PXE_NODISCARD PxeWindow* NpEngine::createWindow(uint32_t width, uint32_t height, const char* title, PxeWindowId windowId)
		{
			if (windowId == PxeInvalidWindowId) {
				PXE_ERROR("Attempted to create PxeWindow using PxeInvalidWindowId as windowId value");
				return nullptr;
			}

			if (_pxeWindows.find(windowId) != _pxeWindows.end()) {
				PXE_WARN("Attempted to create PxeWindow, PxeWindowId already in use");
				return nullptr;
			}

			NpWindow* window = new(std::nothrow) NpWindow(width, height, title, windowId);
			if (!window) {
				PXE_ERROR("Failed to allocate PxeWindow");
				return nullptr;
			}

			if (_primaryWindowId == PxeInvalidWindowId) {
				_primaryWindowId = windowId;
				window->setPrimaryWindow();
			}

			_pxeWindows.emplace(windowId, window);
			return window;
		}

		PXE_NODISCARD PxeWindow* NpEngine::getWindow(PxeWindowId windowId)
		{
			auto it = _pxeWindows.find(windowId);
			if (it != _pxeWindows.end()) return it->second;
			return nullptr;
		}

		void NpEngine::removeWindow(NpWindow& window)
		{
			if (window.getSDLWindow()) {
				uint32_t sdlId = window.getSDLWindowId();
				if (_activeKeyboardWindowId == sdlId)
					_activeKeyboardWindowId = 0;
				if (_activeMouseWindowId == sdlId)
					_activeMouseWindowId = 0;
				if (!_sdlWindows.erase(sdlId)) {
					PXE_ERROR("Failed to remove SDL window from cache");
				}
			}

			if (!_pxeWindows.erase(window.getWindowId())) {
				PXE_ERROR("Failed to remove PxeWindow from cache");
			}
		}

		PXE_NODISCARD PxeLogInterface& NpEngine::getLogInterface()
		{
			return _logInterface;
		}

		PXE_NODISCARD SDL_Window* NpEngine::createSDLWindow(NpWindow& window)
		{
			if (window.getPrimaryWindow()) {
				SDL_Window* sdlWindow = SDL_CreateWindow(window.getWindowTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window.getWindowWidth(), window.getWindowHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
				if (!sdlWindow) {
					PXE_CHECKSDLERROR();
					PXE_FATAL("Failed to Create SDL Window");
				}

				_sdlWindows.emplace(SDL_GetWindowID(sdlWindow), &window);
				initRendering(sdlWindow);
				return sdlWindow;
			}
			else {
				PXE_ERROR("TODO Implement multi window creation");
				return nullptr;
			}
		}

		void NpEngine::destroySDLWindow(SDL_Window* sdlWindow, NpWindow& window)
		{
			if (window.getPrimaryWindow()) {

				//SDL_DestroyWindow(sdlWindow);
				//PXE_CHECKSDLERROR();
				// Hack as we need the primary SDL window until right before shutdown to clean up PxeGlAssets, so store it here
				// and destroy it later in engineShutdown()
				_shutdownFlag = sdlWindow;
			}
			else {
				PXE_ERROR("TODO Implement multi window destruction");
			}
		}

		PXE_NODISCARD ImGuiContext* NpEngine::createGuiContext(NpWindow& window)
		{
			if (window.getPrimaryWindow()) {
				ImGuiContext* ctx = ImGui::CreateContext(&_guiFontAtlas);
				ImGui::SetCurrentContext(ctx);
				ImGui::StyleColorsDark();
				//ImGui::StyleColorsLight();
				// OpenGl context is not needed here as this is not the ImGui docking branch
				ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), nullptr);
				// TODO Select correct versions dynamically
				ImGui_ImplOpenGL3_Init("#version 130");
				return ctx;
			}
			else {
				PXE_ERROR("TODO Implement multi gui context creation");
				return nullptr;
			}
		}

		void NpEngine::destroyGuiContext(ImGuiContext* context, NpWindow& window)
		{
			if (window.getPrimaryWindow()) {
				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplSDL2_Shutdown();
				ImGui::DestroyContext(context);
			}
			else {
				PXE_ERROR("TODO Implement multi gui context destruction");
			}
		}

		void NpEngine::initRendering(SDL_Window* window) {
			_sdlGlContext = SDL_GL_CreateContext(window);
			if (!_sdlGlContext) {
				PXE_CHECKSDLERROR();
				PXE_FATAL("Failed to Create OpenGl Context");
				return;
			}

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
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glFrontFace(GL_CCW);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//glCullFace(GL_NONE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		void NpEngine::deinitRendering() {
			SDL_GL_DeleteContext(_sdlGlContext);
			PXE_CHECKSDLERROR();
			_sdlGlContext = nullptr;
		}

		PxeScene* NpEngine::createScene()
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
		PxeShader* NpEngine::loadShader(const std::filesystem::path& path)
		{
			auto it = _shaderCache.find(path);
			if (it != _shaderCache.end()) {
				it->second->grab();
				return it->second;
			}
			else {
				NpShader* shader = new(std::nothrow) NpShader(path);
				if (shader) {
					_shaderCache.emplace(path, shader);
				}

				return shader;
			}
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

		void NpEngine::processEvents()
		{
			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				// is it a window event
				if (e.type == SDL_WINDOWEVENT)
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

					auto winIt = _sdlWindows.find(e.window.windowID);
					if (winIt != _sdlWindows.end()) {
						if (!winIt->second->pushEvent(e)) {
							PXE_WARN("PxeWindow event buffer overflow");
						}
					}
				}
				// is it a keyboard input event
				else if (e.type >= SDL_KEYDOWN && e.type <= SDL_TEXTEDITING_EXT && e.type != SDL_KEYMAPCHANGED) {
					if (_activeKeyboardWindowId) {
						auto winIt = _sdlWindows.find(_activeKeyboardWindowId);
						if (winIt != _sdlWindows.end()) {
							if (!winIt->second->pushEvent(e)) {
								PXE_WARN("PxeWindow event buffer overflow");
							}
						}
					}
				}
				// is it a mouse/joystick/touch input event
				else if (e.type >= SDL_MOUSEMOTION && e.type <= SDL_MULTIGESTURE) {
					if (_activeMouseWindowId) {
						auto winIt = _sdlWindows.find(_activeMouseWindowId);
						if (winIt != _sdlWindows.end()) {
							if (!winIt->second->pushEvent(e)) {
								PXE_WARN("PxeWindow event buffer overflow");
							}
						}
					}
				}
				// forward all other events to all windows
				else
				{
					for (auto it = _sdlWindows.begin(); it != _sdlWindows.end(); ++it) {
						if (!it->second->pushEvent(e)) {
							PXE_WARN("PxeWindow event buffer overflow");
						}
					}
				}
			}

			// We only use _sdlWindows because there the only windows that can generate SDL events 
			for (auto it = _sdlWindows.begin(); it != _sdlWindows.end(); ++it) {
				it->second->processEvents();
			}
		}

		void NpEngine::processAssetQueue() {
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

		void NpEngine::uninitializeGlAsset(PxeGLAsset& asset)
		{
			if (asset.getAssetStatus() == PxeGLAssetStatus::UNINITIALIZED) return;
			if (asset.getAssetStatus() < PxeGLAssetStatus::INITIALIZED) {
				PXE_ERROR("uninitializeGlAsset called on PxeGlAsset in a pending state");
				return;
			}

			asset._status = PxeGLAssetStatus::PENDING_UNINITIALIZATION;
			// TODO Make this thread safe
			_assetQueue.emplace_back(&asset);
			asset.grab();
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
			// TODO Make this thread safe
			_assetQueue.emplace_back(&asset);
			asset.grab();
		}

		// TODO make this thread safe
		void NpEngine::removeShader(const std::filesystem::path& path)
		{
#ifdef PXE_DEBUG
			if (_shaderCache.erase(path)) {
				PXE_INFO("Removed PxeShader " + path.string() + " from cache");
			}
			else {
				PXE_WARN("Attempted to remove PxeShader " + path.string() + " not in cache");
			}
#else
			_shaderCache.erase(path);
#endif // PXE_DEBUG

		}

		void NpEngine::shutdown() {
			if (_shutdownFlag) return;
			_shutdownFlag = _pxeWindows.find(_primaryWindowId)->second->getSDLWindow();
		}

		// TODO This dose not work properly without creating a primary window
		void NpEngine::shutdownEngine()
		{
			PXE_INFO("PxeEngine shutdown");
			processAssetQueue();
			if (getRefCount() > 1) {
				PXE_ERROR("PxeEngine still referenced, all references to the PxeEngine should be dropped before shutdown, this indicates a memory leak, still " + std::to_string(getRefCount() - 1) + " references");
			}

			SDL_DestroyWindow(static_cast<SDL_Window*>(_shutdownFlag));
			PXE_CHECKSDLERROR();
			PXE_INFO("PxeEngine shutdown complete");
			drop();
		}

		const std::unordered_map<PxeWindowId, NpWindow*>& NpEngine::getWindows()
		{
			return _pxeWindows;
		}

		void NpEngine::newFrame(NpWindow& window)
		{
			if (window.getPrimaryWindow()) {
				window.updateWindowProperties();
				glViewport(0, 0, window.getWindowWidth(), window.getWindowHeight());
				glClearColor(0, 0, 0, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				window.bindGuiContext();
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();
			}
			else {
				PXE_ERROR("TODO Add non primary window frame creation");
			}
		}

		void NpEngine::renderFrame(NpWindow& window)
		{
			const std::list<PxeRenderBase*>& renderList = window.getNpScene()->getRenderList(PxeRenderPass::WORLD_SPACE);
			if (renderList.empty()) return;
			// TODO Move this to proper locations
			glm::mat4 vp(glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f) * glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0), glm::vec3(0, 1, 0)));
			PxeShader* activeShader = nullptr;
			PxeRenderMaterial* activeMaterial = nullptr;
			int32_t mvpLocation = -1;

			for (auto it = renderList.begin(); it != renderList.end(); ++it) {
				PxeRenderObject& renderObject = static_cast<PxeRenderObject&>(**it);
				if (&(renderObject.getRenderMaterial()) != activeMaterial) {
					activeMaterial = &(renderObject.getRenderMaterial());
					if (&(activeMaterial->getShader()) != activeShader) {
						activeShader = &(activeMaterial->getShader());
						activeShader->bind();
						mvpLocation = activeShader->getUniformLocation("u_MVP");
					}

					activeMaterial->applyMaterial();
				}

				activeShader->setUniformM4f(mvpLocation, vp * renderObject.getPositionMatrix());
				renderObject.onRender();
			}
		}
		
		void NpEngine::renderGui(NpWindow& window)
		{
			window.bindGuiContext();
			const std::list<PxeRenderBase*>& renderList = window.getNpScene()->getRenderList(PxeRenderPass::SCREEN_SPACE);
			if (renderList.empty()) return;
			for (auto it = renderList.begin(); it != renderList.end(); ++it) {
				(*it)->onRender();
			}
		}

		void NpEngine::swapFramebuffer(NpWindow& window)
		{
			window.bindGuiContext();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow(window.getSDLWindow());
		}
	}
}