#include "NpWindow.h"

#include <cstring>

#include "NpEngine.h"
#include "NpLogger.h"
#include "backends/imgui_impl_sdl.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "PxeDefaultCamera.h"

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
			_externalFramebuffer = static_cast<uint32_t>(-1);
			_width = width;
			_height = height;
			_flags = 0;
			// Invalid dummy value to force VSync mode refresh
			_vsyncMode = static_cast<int8_t>(0xff);
			setWindowTitle(title);
		}

		NpWindow::~NpWindow()
		{
			// Don't handle _sdlWindow, _sdlGlContext, _guiContext, _renderTexture
			// as all should be managed by the NpEngine in initializeGl and uninitializeGl
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

		void NpWindow::setPrimaryWindow()
		{
			std::unique_lock lock(_windowMutex);
			setFlag(PRIMARY_WINDOW);
		}

		void NpWindow::initializeGl()
		{
			NpEngine& engine = NpEngine::getInstance();
			engine.initializeWindow(*this);
		}

		void NpWindow::uninitializeGl()
		{
			NpEngine& engine = NpEngine::getInstance();
			engine.uninitializeWindow(*this);
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

		void NpWindow::bindGuiContext()
		{
#ifdef PXE_DEBUG
			if (!_guiContext) {
				PXE_WARN("Binding invalid GUI context");
			}
#endif // PXE_DEBUG

			ImGui::SetCurrentContext(_guiContext);
		}

		void NpWindow::updateSDLWindow()
		{
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
		}

		void NpWindow::setVsyncMode(int8_t mode)
		{
			if (_vsyncMode != mode) {
				_vsyncMode = mode;
				SDL_GL_SetSwapInterval(_vsyncMode);
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

		void NpWindow::acquireWriteLock()
		{
			_windowMutex.lock();
		}

		void NpWindow::releaseWriteLock()
		{
			_windowMutex.unlock();
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

		PXE_NODISCARD bool NpWindow::getPrimaryWindow() const
		{
			return getFlag(PRIMARY_WINDOW);
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
			_width = width;
			_height = height;
			setFlag(SIZE_CHANGED);
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

		void NpWindow::setCamera(PxeCamera* camera)
		{
			std::unique_lock lock(_windowMutex);
			if (_camera) _camera->drop();
			_camera = camera;
			if (_camera) _camera->grab();
		}

		PXE_NODISCARD PxeCamera* NpWindow::getCamera() const
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