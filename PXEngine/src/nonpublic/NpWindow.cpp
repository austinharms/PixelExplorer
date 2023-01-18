#include "NpWindow.h"

#include <cstring>

#include "NpEngine.h"
#include "NpLogger.h"
#include "backends/imgui_impl_sdl.h"

namespace pxengine {
	namespace nonpublic {
		NpWindow::NpWindow(int32_t width, int32_t height, const char* title, PxeWindowId windowId)
		{
			_sdlWindow = nullptr;
			_guiContext = nullptr;
			_scene = nullptr;
			_title = nullptr;
			_width = width;
			_height = height;
			_windowId = windowId;
			_swapInterval = 1;
			_shouldClose = false;
			_primary = false;
			_propertyFlags = 0;
			setWindowTitle(title);
		}

		NpWindow::~NpWindow()
		{
			if (_scene)
				_scene->drop();
			if (_title)
				free(_title);
		}

		PXE_NODISCARD SDL_Window* NpWindow::getSDLWindow() const
		{
			return _sdlWindow;
		}

		void NpWindow::setSwapInterval(int8_t interval)
		{
			_swapInterval = interval;
			_propertyFlags |= WINDOW_SWAP_CHANGED;
		}

		int8_t NpWindow::getSwapInterval()
		{
			return _swapInterval;
		}

		bool NpWindow::getShouldClose() const
		{
			return _shouldClose;
		}

		void NpWindow::resetShouldClose()
		{
			_shouldClose = false;
		}

		// TODO does changing window visibility require the GL Context?
		void NpWindow::setWindowShown(bool show)
		{
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
			_shouldClose = true;
		}

		void NpWindow::setPrimaryWindow()
		{
			_primary = true;
		}

		void NpWindow::initializeGl()
		{
			NpEngine& engine = NpEngine::getInstance();
			_sdlWindow = engine.createSDLWindow(*this);
			if (!_sdlWindow) {
				PXE_ERROR("Failed to initialize PxeWindow, invalid SDL window");
				setErrorStatus();
				setShouldClose();
				return;
			}

			_guiContext = engine.createGuiContext(*this);
			if (!_guiContext) {
				PXE_ERROR("Failed to initialize PxeWindow, invalid GUI context");
				setErrorStatus();
				setShouldClose();
				return;
			}

			if (getPrimaryWindow()) {
				PXE_INFO("Created Primary PxeWindow");
			}
			else {
				PXE_INFO("Created PxeWindow");
			}
		}

		void NpWindow::uninitializeGl()
		{
			NpEngine& engine = NpEngine::getInstance();
			engine.removeWindow(*this);
			if (_guiContext) {
				engine.destroyGuiContext(_guiContext, *this);
				_guiContext = nullptr;
			}

			if (_sdlWindow) {
				engine.destroySDLWindow(_sdlWindow, *this);
				_sdlWindow = nullptr;
			}
		}

		void NpWindow::bindGuiContext()
		{
#ifdef PXE_DEBUG
			if (!_guiContext) {
				PXE_WARN("Attempted to bind invalid gui context");
				return;
			}
#endif // PXE_DEBUG

			ImGui::SetCurrentContext(_guiContext);
		}

		void NpWindow::updateWindowProperties()
		{
			if (_propertyFlags & WINDOW_TITLE_CHANGED && _sdlWindow) {
				SDL_SetWindowTitle(_sdlWindow, _title);
				_propertyFlags ^= WINDOW_TITLE_CHANGED;
			}

			if (_propertyFlags & WINDOW_SIZE_CHANGED && _sdlWindow) {
				SDL_SetWindowSize(_sdlWindow, _width, _height);
				_propertyFlags ^= WINDOW_SIZE_CHANGED;
			}

			if (_propertyFlags & WINDOW_SWAP_CHANGED && _sdlWindow) {
				SDL_GL_SetSwapInterval(_swapInterval);
				_propertyFlags ^= WINDOW_SWAP_CHANGED;
			}

			if (_sdlWindow)
				SDL_GetWindowSizeInPixels(_sdlWindow, &_width, &_height);
		}

		void NpWindow::processEvents()
		{
			if (_eventBuffer.empty()) return;
			bindGuiContext();
			while (!_eventBuffer.empty())
			{
				SDL_Event evt = _eventBuffer.peek<SDL_Event>();
				_eventBuffer.pop<SDL_Event>();
				if (evt.type == SDL_QUIT || evt.type == SDL_WINDOWEVENT && static_cast<SDL_WindowEventID>(evt.window.event) == SDL_WINDOWEVENT_CLOSE) {
					setShouldClose();
				}

				if (getAssetStatus() == PxeGLAssetStatus::ERROR) continue;

				// Not great using this here as technically the underlaying implementation could be anything
				// as it is created by the NpEngine, but it does not make senses to have the NpEngine handle window events
				// so this is the lesser evil
				ImGui_ImplSDL2_ProcessEvent(&evt);
			}
		}

		void NpWindow::setScene(PxeScene* scene)
		{
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
			return _scene;
		}

		PXE_NODISCARD bool NpWindow::getPrimaryWindow() const
		{
			return _primary;
		}

		PXE_NODISCARD PxeWindowId NpWindow::getWindowId() const
		{
			return _windowId;
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
			_width = width;
			_height = height;
			_propertyFlags |= WINDOW_SIZE_CHANGED;
		}

		PXE_NODISCARD const char* NpWindow::getWindowTitle() const
		{
			return _title;
		}

		void NpWindow::setWindowTitle(const char* title)
		{
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
			_propertyFlags |= WINDOW_TITLE_CHANGED;
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

		bool NpWindow::pushEvent(const SDL_Event& event)
		{
			return _eventBuffer.insert(event);
		}
	}
}