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
#include "PxeRenderTexture.h"

namespace pxengine {
	namespace nonpublic {
		// Used to store data required for rendering for non primary windows
		struct NpExtendedWindowData
		{
			SDL_GLContext GlContext;
			PxeRenderTexture* RenderTexture;
			uint32_t ExternalFramebuffer;
		};

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
			_primaryWindow = nullptr;
			_shutdownFlag = 0;
			_guiBackendReferenceCount = 0;
			_guiBackend = nullptr;
			_vsyncMode = 0;

			_inputManager = new(std::nothrow) NpInputManager();
			if (!_inputManager) {
				PXE_FATAL("Failed to create PxeInputManager for PxeEngine");
			}

			initSDL();
			initPhysics();
			PXE_INFO("PxeEngine Ready");
		}

		NpEngine::~NpEngine()
		{
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
#ifndef PXE_DISABLE_GL_INFO_LOG
				PXE_INFO(messageStr);
#endif // !PXE_DISABLE_GL_INFO_LOG
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
#define PXE_PHYSICS_TRACK_ALLOCATIONS true
#else
#define PXE_PHYSICS_PVD_POINTER nullptr
#define PXE_PHYSICS_TRACK_ALLOCATIONS false
#endif
			_physPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *_physFoundation, _physScale, PXE_PHYSICS_TRACK_ALLOCATIONS, PXE_PHYSICS_PVD_POINTER);
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

			if (_primaryWindow == nullptr) {
				_primaryWindow = window;
				window->setPrimaryWindow();
			}

			return window;
		}

		void NpEngine::removeWindow(NpWindow& window)
		{
			if (window.getSDLWindow()) {
				uint32_t sdlId = window.getSDLWindowId();
				_inputManager->clearActiveWindow(&window);
				if (!_sdlWindows.erase(sdlId)) {
					PXE_ERROR("Failed to remove SDL window from cache");
				}
			}

			if (&window == _primaryWindow)
				_primaryWindow = nullptr;
		}

		PXE_NODISCARD PxeLogInterface& NpEngine::getLogInterface()
		{
			return _logInterface;
		}

		PXE_NODISCARD SDL_Window* NpEngine::createSDLWindow(NpWindow& window)
		{
			SDL_Window* sdlWindow = SDL_CreateWindow(window.getWindowTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window.getWindowWidth(), window.getWindowHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
			if (!sdlWindow) {
				PXE_CHECKSDLERROR();
				PXE_ERROR("Failed to create SDL window");
				if (window.getPrimaryWindow()) {
					PXE_FATAL("Failed to create primary window");
				}
				return nullptr;
			}

			if (window.getPrimaryWindow()) {
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
			}
			else {
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
			}

			SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
			if (!glContext) {
				PXE_CHECKSDLERROR();
				PXE_ERROR("Failed to create OpenGl context");
				SDL_DestroyWindow(sdlWindow);
				if (window.getPrimaryWindow()) {
					PXE_FATAL("Failed to create primary window");
				}
				return nullptr;
			}

			if (window.getPrimaryWindow()) {
				_primaryGlContext = glContext;
				initPrimaryGlContext();
			}
			else {
				NpExtendedWindowData* windowData = new(std::nothrow) NpExtendedWindowData();
				if (!windowData) {
					PXE_ERROR("Failed to allocate window data");
					SDL_GL_DeleteContext(glContext);
					SDL_DestroyWindow(sdlWindow);
					bindPrimaryGlContext();
					return nullptr;
				}

				windowData->GlContext = glContext;
				windowData->RenderTexture = new(std::nothrow) PxeRenderTexture(window.getWindowWidth(), window.getWindowHeight());
				if (!windowData->RenderTexture) {
					PXE_ERROR("Failed to create PxeRenderTexture");
					delete windowData;
					SDL_GL_DeleteContext(glContext);
					SDL_DestroyWindow(sdlWindow);
					bindPrimaryGlContext();
					return nullptr;
				}

				bindPrimaryGlContext();
				forceAssetInit(*(windowData->RenderTexture));
				SDL_GL_MakeCurrent(sdlWindow, glContext);

				glGenFramebuffers(1, &(windowData->ExternalFramebuffer));
				glBindFramebuffer(GL_FRAMEBUFFER, windowData->ExternalFramebuffer);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, windowData->RenderTexture->getGlTextureId(), 0);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					PXE_ERROR("Failed to create window glFramebuffer: " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
					windowData->RenderTexture->drop();
					delete windowData;
					SDL_GL_DeleteContext(glContext);
					SDL_DestroyWindow(sdlWindow);
					bindPrimaryGlContext();
					return nullptr;
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, windowData->ExternalFramebuffer);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				bindPrimaryGlContext();
				SDL_SetWindowData(sdlWindow, EXTENDED_WINDOW_DATA_NAME, windowData);
			}

			_sdlWindows.emplace(SDL_GetWindowID(sdlWindow), &window);
			return sdlWindow;
		}

		void NpEngine::destroySDLWindow(SDL_Window* sdlWindow, NpWindow& window)
		{
			if (window.getPrimaryWindow()) {

				// Hack as we need the primary SDL window until right before shutdown to clean up PxeGlAssets, so store it here
				// and destroy it later in engineShutdown()
				_shutdownFlag = sdlWindow;
			}
			else {
				NpExtendedWindowData* windowData = static_cast<NpExtendedWindowData*>(SDL_GetWindowData(sdlWindow, EXTENDED_WINDOW_DATA_NAME));
				windowData->RenderTexture->drop();
				SDL_GL_DeleteContext(windowData->GlContext);
				PXE_CHECKSDLERROR();
				SDL_DestroyWindow(sdlWindow);
				PXE_CHECKSDLERROR();
				delete windowData;
			}
		}

		PXE_NODISCARD ImGuiContext* NpEngine::createGuiContext(NpWindow& window)
		{
			ImGuiContext* ctx = ImGui::CreateContext(&_guiFontAtlas);
			ImGui::SetCurrentContext(ctx);
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();
			// OpenGl context is not needed here as this is not the ImGui docking branch
			ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), nullptr);

			if (window.getPrimaryWindow()) {
				// TODO Select correct versions dynamically
				ImGui_ImplOpenGL3_Init("#version 130");
				_guiBackendReferenceCount = 1;
				_guiBackend = ImGui::GetIO().BackendRendererUserData;
			}
			else {
				// The primary window should always be created first and this should not happen but just in case
				if (!_guiBackend) {
					PXE_ERROR("Failed to get gui backend");
					ImGui_ImplSDL2_Shutdown();
					ImGui::DestroyContext(ctx);
					return nullptr;
				}

				ImGuiIO& io = ImGui::GetIO();
				// Crappy hack to allow using one backend across all gui contexts
				_guiBackendReferenceCount += 1;
				io.BackendRendererUserData = _guiBackend;
				io.BackendRendererName = "imgui_impl_opengl3";
				if (*((uint32_t*)_guiBackend) >= 320)
					io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
			}

			return ctx;
		}

		void NpEngine::destroyGuiContext(ImGuiContext* context, NpWindow& window)
		{
			ImGui::SetCurrentContext(context);
			if (--_guiBackendReferenceCount == 0) {
				ImGui_ImplOpenGL3_Shutdown();
				_guiBackend = nullptr;
			}
			else {
				ImGuiIO& io = ImGui::GetIO();
				io.BackendRendererName = nullptr;
				io.BackendRendererUserData = nullptr;
			}

			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext(context);
		}

		PXE_NODISCARD NpInputManager& NpEngine::getNpInputManager() const
		{
			return *_inputManager;
		}

		void NpEngine::initPrimaryGlContext() {
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

		void NpEngine::forceAssetInit(PxeGLAsset& asset)
		{
			asset.initialize();
			if (_assetQueue.remove(&asset)) {
				asset.drop();
			}
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

		void NpEngine::setVSyncMode(int8_t mode)
		{
			_vsyncMode = mode;
		}

		PXE_NODISCARD int8_t NpEngine::getVSyncMode() const
		{
			return _vsyncMode;
		}

		PXE_NODISCARD PxeInputManager& NpEngine::getInputManager() const
		{
			return static_cast<PxeInputManager&>(*_inputManager);
		}

		PXE_NODISCARD float NpEngine::getDeltaTime() const
		{
			return _deltaTime;
		}

		PXE_NODISCARD physx::PxFoundation* NpEngine::getPhysicsFoundation() const
		{
			return _physFoundation;
		}

		PXE_NODISCARD physx::PxPhysics* NpEngine::getPhysicsBase() const
		{
			return _physPhysics;
		}

		PXE_NODISCARD physx::PxCooking* NpEngine::getPhysicsCooking() const
		{
			return _physCooking;
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
			_shutdownFlag = _primaryWindow->getSDLWindow();
		}

		// TODO This dose not work properly without creating a primary window
		void NpEngine::shutdownEngine()
		{
			PXE_INFO("PxeEngine shutdown");
			SDL_Window* primarySDLWindow = static_cast<SDL_Window*>(_shutdownFlag);
			_shutdownFlag = 0;
			SDL_GL_MakeCurrent(primarySDLWindow, _primaryGlContext);
			processAssetQueue();
			if (getRefCount() > 1) {
				PXE_ERROR("PxeEngine still referenced, all references to the PxeEngine should be dropped before shutdown, this indicates a memory leak, still " + std::to_string(getRefCount() - 1) + " references");
			}

			SDL_GL_DeleteContext(_primaryGlContext);
			PXE_CHECKSDLERROR();
			_primaryGlContext = nullptr;
			SDL_DestroyWindow(primarySDLWindow);
			PXE_CHECKSDLERROR();
			PXE_INFO("PxeEngine shutdown complete");
			drop();
		}

		const std::unordered_map<uint32_t, NpWindow*>& NpEngine::getWindows()
		{
			return _sdlWindows;
		}

		void NpEngine::newFrame(NpWindow& window)
		{
			window.updateSDLWindowProperties();
			if (window.getPrimaryWindow()) {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			else {
				NpExtendedWindowData* windowData = static_cast<NpExtendedWindowData*>(SDL_GetWindowData(window.getSDLWindow(), EXTENDED_WINDOW_DATA_NAME));
				windowData->RenderTexture->bind();
				windowData->RenderTexture->resize(window.getWindowWidth(), window.getWindowHeight());
			}

			glViewport(0, 0, window.getWindowWidth(), window.getWindowHeight());
			glClearColor(0, 0, 0, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			window.bindGuiContext();
			ImGui_ImplOpenGL3_NewFrame();
			ImGuiIO& io = ImGui::GetIO();
			if (_inputManager->getMouseFocusedWindow() == &window) {
				io.ConfigFlags &= !ImGuiConfigFlags_NoMouseCursorChange;
			}
			else {
				io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			}

			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();
		}

		void NpEngine::renderFrame(NpWindow& window, PxeRenderPass pass)
		{
			NpScene* scene = window.getNpScene();
			const std::list<PxeRenderBase*>& renderList = scene->getRenderList(pass);
			if (renderList.empty()) return;
			PxeShader* activeShader = nullptr;
			PxeRenderMaterial* activeMaterial = nullptr;
			int32_t mvpLocation = -1;
			PxeCamera& camera = *window.getCamera();
			camera.setWindowSize(window.getWindowWidth(), window.getWindowHeight());
			glm::mat4 pvMatrix(camera.getPVMatrix());
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

				activeShader->setUniformM4f(mvpLocation, pvMatrix * renderObject.getPositionMatrix());
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
			if (!window.getPrimaryWindow()) {
				NpExtendedWindowData* windowData = static_cast<NpExtendedWindowData*>(SDL_GetWindowData(window.getSDLWindow(), EXTENDED_WINDOW_DATA_NAME));
				SDL_Window* sdlWindow = window.getSDLWindow();
				windowData->RenderTexture->unbind();
				SDL_GL_MakeCurrent(sdlWindow, windowData->GlContext);
				glViewport(0, 0, window.getWindowWidth(), window.getWindowHeight());
				glBlitFramebuffer(0, 0, window.getWindowWidth(), window.getWindowHeight(), 0, 0, window.getWindowWidth(), window.getWindowHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}

			window.setVsyncMode(_vsyncMode);
			SDL_GL_SwapWindow(window.getSDLWindow());
		}

		void NpEngine::bindPrimaryGlContext()
		{
			if (_primaryWindow && _primaryGlContext) {
				SDL_GL_MakeCurrent(_primaryWindow->getSDLWindow(), _primaryGlContext);
			}
		}

		void NpEngine::setDeltaTime(float dt)
		{
			_deltaTime = dt;
		}
	}
}