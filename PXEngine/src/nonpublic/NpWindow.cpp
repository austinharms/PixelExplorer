#include "NpWindow.h"

#include <cstring>
#include <string>

#include "NpLogger.h"
#include "NpEngine.h"
#include "NpRenderPipeline.h"
#include "PxeDefaultCamera.h"
#include "backends/imgui_impl_sdl.h"
#include "glm/ext/matrix_clip_space.hpp"

namespace pxengine {
	namespace nonpublic {
		NpWindow::NpWindow(int32_t width, int32_t height, const char* title)
		{
			_userData = nullptr;
			_title = nullptr;
			_scene = nullptr;
			_camera = PxeDefaultCamera::createCamera(glm::radians(90.0f), 0.1f, 100.0f);
			_sdlWindow = nullptr;
			_sdlGlContext = nullptr;
			_guiContext = nullptr;
			_renderTexture = nullptr;
			_internalFramebuffer = 0;
			_width = width;
			_height = height;
			_flags = 0;
			// Invalid dummy value
			_vsyncMode = -128;
			setWindowTitle(title);
		}

		NpWindow::~NpWindow()
		{
			// Don't handle _sdlWindow, _sdlGlContext, _guiContext, _renderTexture
			// as all should be managed in initializeGl and uninitializeGl
			_windowMutex.lock();
			if (_scene) _scene->drop();
			if (_title) free(_title);
			_windowMutex.unlock();
		}

		PXE_NODISCARD SDL_Window* NpWindow::getSDLWindow() const
		{
			return _sdlWindow;
		}

		bool NpWindow::getShouldClose() const
		{
			std::shared_lock lock(_windowMutex);
			return getFlag(WINDOW_CLOSE);
		}

		void NpWindow::resetShouldClose()
		{
			std::unique_lock lock(_windowMutex);
			clearFlag(WINDOW_CLOSE);
		}

		// TODO does changing window visibility require the GL Context?
		void NpWindow::setWindowShown(bool show)
		{
			std::unique_lock lock(_windowMutex);
#ifdef PXE_DEBUG
			if (!_sdlWindow) {
				PXE_WARN("Attempted to change visibility of invalid SDL window");
				return;
			}
#endif // PXE_DEBUG

			if (show) {
				SDL_ShowWindow(_sdlWindow);
			}
			else {
				SDL_HideWindow(_sdlWindow);
			}
		}

		bool NpWindow::getWindowShown() const
		{
			std::shared_lock lock(_windowMutex);
#ifdef PXE_DEBUG
			if (!_sdlWindow) {
				PXE_WARN("Attempted to get visibility of invalid SDL window");
				return false;
			}
#endif // PXE_DEBUG
			return SDL_GetWindowFlags(_sdlWindow) & SDL_WINDOW_SHOWN;
		}

		void NpWindow::setShouldClose()
		{
			std::unique_lock lock(_windowMutex);
			setFlag(WINDOW_CLOSE);
		}

		void NpWindow::initializeGl()
		{
			std::lock_guard lock(_windowMutex);
			NpEngine& engine = NpEngine::getInstance();
			_sdlWindow = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
			if (!_sdlWindow) {
				PXE_CHECKSDLERROR();
				PXE_ERROR("Failed to create PxeWindow's SDL_Window");
				setErrorStatus();
				setShouldClose();
				return;
			}

			if (engine.hasPrimaryGlContext()) {
				_renderTexture = new(std::nothrow) PxeRenderTexture(_width, _height, true);
				if (!_renderTexture) {
					PXE_ERROR("Failed to create PxeWindow's PxeRenderTexture");
					setErrorStatus();
					setShouldClose();
					return;
				}

				engine.forceInitializeGlAsset(*_renderTexture);
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
				PXE_CHECKSDLERROR();
			}
			else {
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
				PXE_CHECKSDLERROR();
			}

			_sdlGlContext = SDL_GL_CreateContext(_sdlWindow);
			if (!_sdlGlContext) {
				PXE_CHECKSDLERROR();
				PXE_ERROR("Failed to create PxeWindow's OpenGl context");
				setErrorStatus();
				setShouldClose();
				return;
			}

			engine.initGlContext();
			// TODO Can this fail gracefully?
			engine.getRenderPipeline().grab();
			_guiContext = ImGui::CreateContext(engine.getNpRenderPipeline().getFontAtlas());
			//if (!_guiContext) {
			//	PXE_ERROR("Failed to create window GUI context");
			//	setErrorStatus();
			//	setShouldClose();
			//	return;
			//}

			ImGui::SetCurrentContext(_guiContext);
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
			//ImGui::StyleColorsLight();

			// TODO Can this fail gracefully?
			// OpenGl context is not needed here as this is not the ImGui docking branch
			ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, nullptr);
			//if (!ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, nullptr)) {
			//	PXE_ERROR("Failed to create window GUI context");
			//	setErrorStatus();
			//	setShouldClose();
			//	return;
			//}

			engine.getNpRenderPipeline().installGuiBackend();
			// TODO De-duplicate this block
			if (!engine.hasPrimaryGlContext()) {

				_renderTexture = new(std::nothrow) PxeRenderTexture(_width, _height, true);
				if (!_renderTexture) {
					PXE_ERROR("Failed to create PxeWindow's PxeRenderTexture");
					setErrorStatus();
					setShouldClose();
					return;
				}

				// We assume this context will become the primary OpenGl context
				engine.forceInitializeGlAsset(*_renderTexture);
			}

			if (_renderTexture->getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
				PXE_ERROR("Failed to initialize PxeWindow's PxeRenderTexture");
				setErrorStatus();
				setShouldClose();
				return;
			}

			if (!createFramebuffer()) {
				PXE_ERROR("Failed to create PxeWindow's framebuffer");
				setErrorStatus();
				setShouldClose();
				return;
			}

			_vsyncMode = engine.getRenderPipeline().getVSyncMode();
			setFlag(SWAP_CHANGED);
			engine.registerWindow(*this);
			engine.bindPrimaryGlContext();
		}

		void NpWindow::uninitializeGl()
		{
			NpEngine& engine = NpEngine::getInstance();
			engine.unregisterWindow(*this);
			bindGlContext();
			deleteFramebuffer();
			engine.bindPrimaryGlContext();
			if (_renderTexture) {
				_renderTexture->drop();
				_renderTexture = nullptr;
			}

			bindGuiContext();
			engine.getNpRenderPipeline().uninstallGuiBackend();
			ImGui_ImplSDL2_Shutdown();
			engine.getRenderPipeline().drop();
			ImGui::DestroyContext(_guiContext);
			_guiContext = nullptr;

			if (_sdlGlContext) {
				SDL_GL_DeleteContext(_sdlGlContext);
				_sdlGlContext = nullptr;
			}

			if (_sdlWindow) {
				SDL_DestroyWindow(_sdlWindow);
				_sdlWindow = nullptr;
			}
		}

		bool NpWindow::getFlag(WindowFlag flag) const
		{
			return _flags & (uint8_t)flag;
		}

		void NpWindow::clearFlag(WindowFlag flag)
		{
			_flags &= ~(uint8_t)flag;
		}

		void NpWindow::setFlag(WindowFlag flag, bool value)
		{
			_flags &= ~(uint8_t)flag;
			if (value)
				_flags |= (uint8_t)flag;
		}

		void NpWindow::setFlag(WindowFlag flag)
		{
			_flags |= (uint8_t)flag;
		}

		void NpWindow::deleteFramebuffer()
		{
			glDeleteFramebuffers(1, &_internalFramebuffer);
			_internalFramebuffer = 0;
		}

		bool NpWindow::createFramebuffer()
		{
			if (_internalFramebuffer != 0) {
				PXE_WARN("Called create framebuffer with existing framebuffer, deleting old framebuffer");
				deleteFramebuffer();
			}

			if (!_renderTexture || _renderTexture->getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
				PXE_ERROR("Failed to create PxeWindow's glFramebuffer, invalid PxeRenderTexture");
				return false;
			}

			glGenFramebuffers(1, &_internalFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, _internalFramebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexture->getGlTextureId(), 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				deleteFramebuffer();
				PXE_ERROR("Failed to create PxeWindow's glFramebuffer, Framebuffer status: " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
				return false;
			}

			return true;
		}

		void NpWindow::bindGuiContext()
		{
#ifdef PXE_DEBUG
			if (!_guiContext) {
				PXE_WARN("Binding invalid GUI context");
			}
#endif // PXE_DEBUG

			ImGui::SetCurrentContext(_guiContext);
		}

		void NpWindow::prepareForRender()
		{
			NpEngine& engine = NpEngine::getInstance();
			std::unique_lock lock(_windowMutex);
			if (!_sdlWindow) return;
			if (getFlag(TITLE_CHANGED)) {
				SDL_SetWindowTitle(_sdlWindow, _title);
				clearFlag(TITLE_CHANGED);
			}

			if (getFlag(SIZE_CHANGED)) {
				SDL_SetWindowSize(_sdlWindow, _width, _height);
				clearFlag(SIZE_CHANGED);
			}

			SDL_GetWindowSizeInPixels(_sdlWindow, &_width, &_height);
			_renderTexture->resize(_width, _height);
			_camera->setWindowSize(_width, _height);
			bindGuiContext();
			ImGuiIO& io = ImGui::GetIO();
			if (engine.getInputManager().getMouseFocusedWindow() == this) {
				io.ConfigFlags &= !ImGuiConfigFlags_NoMouseCursorChange;
			}
			else {
				io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			}

			ImGui_ImplSDL2_NewFrame();
		}

		void NpWindow::bindGlContext()
		{
			if (_sdlWindow && _sdlGlContext)
				SDL_GL_MakeCurrent(_sdlWindow, _sdlGlContext);
		}

		void NpWindow::setVsyncMode(int8_t mode)
		{
			std::unique_lock lock(_windowMutex);
			if (_vsyncMode != mode) {
				_vsyncMode = mode;
				setFlag(SWAP_CHANGED);
			}
		}

		void NpWindow::acquireReadLock()
		{
			_windowMutex.lock_shared();
		}

		void NpWindow::releaseReadLock()
		{
			_windowMutex.unlock_shared();
		}

		void NpWindow::swapFramebuffers()
		{
			NpEngine& engine = NpEngine::getInstance();
			setVsyncMode(engine.getRenderPipeline().getVSyncMode());
			std::unique_lock lock(_windowMutex);
			if (getFlag(SIZE_CHANGED)) {
				PXE_WARN("PxeWindow size changed during frame render, skipping frame");
				return;
			}

			bindGlContext();
			if (getFlag(SWAP_CHANGED)) {
				SDL_GL_SetSwapInterval(_vsyncMode);
				clearFlag(SWAP_CHANGED);
			}

			glBindFramebuffer(GL_READ_FRAMEBUFFER, _internalFramebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glViewport(0, 0, _width, _height);
			glDisable(GL_SCISSOR_TEST);
			glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			SDL_GL_SwapWindow(_sdlWindow);
			engine.bindPrimaryGlContext();
		}

		void NpWindow::setScene(PxeScene* scene)
		{
			std::unique_lock lock(_windowMutex);
			if (_scene)
				_scene->drop();
			// TODO Add "magic" values check like SDL?
			_scene = dynamic_cast<NpScene*>(scene);
			if (_scene) {
				_scene->grab();
			}
			else if (scene) {
				PXE_WARN("Scene parameter did not inherit from NpScene, Scene set to nullptr");
			}
		}

		PxeScene* NpWindow::getScene() const
		{
			std::shared_lock lock(_windowMutex);
			return _scene;
		}

		PXE_NODISCARD int32_t NpWindow::getWindowWidth() const
		{
			return _width;
		}

		PXE_NODISCARD int32_t NpWindow::getWindowHeight() const
		{
			return _height;
		}

		void NpWindow::setWindowSize(int32_t width, int32_t height)
		{
			std::unique_lock lock(_windowMutex);
			if (_width != width || _height != height) {
				_width = width;
				_height = height;
				setFlag(SIZE_CHANGED);
			}
		}

		PXE_NODISCARD const char* NpWindow::getWindowTitle() const
		{
			std::shared_lock lock(_windowMutex);
			return _title;
		}

		void NpWindow::setWindowTitle(const char* title)
		{
			std::unique_lock lock(_windowMutex);
			if (!title) {
				PXE_WARN("Attempted to set title using invalid string");
				return;
			}

			size_t length = strlen(title);
			if (length > 128) {
				PXE_ERROR("Failed to set window title, maximum length of 128 chars");
				return;
			}

			char* newTitle = static_cast<char*>(calloc(length + 1, sizeof(char)));
			if (!newTitle) {
				PXE_ERROR("Failed to set window title, failed to allocate title buffer");
				return;
			}

			memcpy(newTitle, title, (length + 1) * sizeof(char));
			if (_title)
				free(_title);
			_title = newTitle;
			setFlag(TITLE_CHANGED);
		}

		void NpWindow::setCamera(PxeCameraInterface* camera)
		{
			std::unique_lock lock(_windowMutex);
			if (_camera) _camera->drop();
			_camera = camera;
			if (_camera) _camera->grab();
		}

		PXE_NODISCARD PxeCameraInterface* NpWindow::getCamera() const
		{
			std::shared_lock lock(_windowMutex);
			return _camera;
		}

		uint32_t NpWindow::getSDLWindowId() const
		{
#ifdef PXE_DEBUG
			if (!_sdlWindow) {
				PXE_WARN("Attempted to get id of invalid SDL window");
				return static_cast<uint32_t>(-1);
			}
#endif // PXE_DEBUG

			return SDL_GetWindowID(_sdlWindow);
		}

		PXE_NODISCARD NpScene* NpWindow::getNpScene() const
		{
			return _scene;
		}

		PXE_NODISCARD PxeRenderTexture* NpWindow::getRenderTexture() const
		{
			return _renderTexture;
		}

		PXE_NODISCARD void* NpWindow::getUserData() const {
			std::shared_lock lock(_windowMutex);
			return _userData;
		}

		void NpWindow::setUserData(void* data) {
			std::unique_lock lock(_windowMutex);
			_userData = data;
		}
	}
}