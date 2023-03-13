#include "PxeWindow.h"

#include <cstring>
#include <string>
#include <shared_mutex>

#include "PxeRenderTexture.h"
#include "imgui.h"
#include "SDL_video.h"
#include "PxeEngine.h"
#include "PxeLogger.h"
#include "PxeDefaultCamera.h"
#include "backends/imgui_impl_sdl.h"
#include "glm/trigonometric.hpp"

namespace pxengine {
	struct PxeWindow::Impl {
		mutable std::shared_mutex _windowMutex;
		char* _title;
		PxeScene* _scene;
		PxeCamera* _camera;
		SDL_Window* _sdlWindow;
		SDL_GLContext _sdlGlContext;
		ImGuiContext* _guiContext;
		PxeRenderTexture* _renderTexture;
		uint32_t _internalFramebuffer;
		int32_t _width;
		int32_t _height;
		uint8_t _flags;
		int8_t _vsyncMode;
	};

	PxeWindow::PxeWindow(int32_t width, int32_t height, const char* title) : PxeGLAsset(true)
	{
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeWindow::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeWindow::STORAGE_ALIGN need be changed");

		userData = nullptr;
		new(&_storage) Impl();
		Impl& imp = impl();
		imp._title = nullptr;
		imp._scene = nullptr;
		imp._camera = PxeDefaultCamera::createCamera(glm::radians(90.0f), 0.1f, 100.0f);
		imp._sdlWindow = nullptr;
		imp._sdlGlContext = nullptr;
		imp._guiContext = nullptr;
		imp._renderTexture = nullptr;
		imp._internalFramebuffer = 0;
		imp._width = width;
		imp._height = height;
		imp._flags = 0;
		// Invalid dummy value
		imp._vsyncMode = -128;
		setWindowTitle(title);
	}

	PxeWindow::~PxeWindow()
	{
		Impl& imp = impl();
		// Don't handle _sdlWindow, _sdlGlContext, _guiContext, _renderTexture
		// as all should be managed in initializeGl and uninitializeGl
		imp._windowMutex.lock();
		if (imp._scene) imp._scene->drop();
		if (imp._title) free(imp._title);
		if (imp._camera) imp._camera->drop();
		imp._windowMutex.unlock();
		imp.~Impl();
	}

	void PxeWindow::onPrepareForRender()
	{
		bindGuiContext();
		ImGuiIO& io = ImGui::GetIO();
		if (PxeEngine::getInstance().getInputManager().getMouseFocusedWindow() == this) {
			io.ConfigFlags &= !ImGuiConfigFlags_NoMouseCursorChange;
		}
		else {
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		}

		ImGui_ImplSDL2_NewFrame();
	}

	void PxeWindow::onRenderCompleted()
	{
		ImGui::SetCurrentContext(nullptr);
	}

	PXE_NODISCARD SDL_Window* PxeWindow::getSDLWindow() const
	{
		return impl()._sdlWindow;
	}

	bool PxeWindow::getShouldClose() const
	{
		std::shared_lock lock(impl()._windowMutex);
		return getFlag(WINDOW_CLOSE);
	}

	void PxeWindow::resetShouldClose()
	{
		std::unique_lock lock(impl()._windowMutex);
		clearFlag(WINDOW_CLOSE);
	}

	// TODO does changing window visibility require the GL Context?
	void PxeWindow::setWindowShown(bool show)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowMutex);
#ifdef PXE_DEBUG
		if (!imp._sdlWindow) {
			PXE_WARN("Attempted to change visibility of invalid SDL window");
			return;
		}
#endif // PXE_DEBUG

		if (show) {
			SDL_ShowWindow(imp._sdlWindow);
		}
		else {
			SDL_HideWindow(imp._sdlWindow);
		}
	}

	bool PxeWindow::getWindowShown() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._windowMutex);
#ifdef PXE_DEBUG
		if (!imp._sdlWindow) {
			PXE_WARN("Attempted to get visibility of invalid SDL window");
			return false;
		}
#endif // PXE_DEBUG
		return SDL_GetWindowFlags(imp._sdlWindow) & SDL_WINDOW_SHOWN;
	}

	void PxeWindow::setShouldClose()
	{
		std::unique_lock lock(impl()._windowMutex);
		setFlag(WINDOW_CLOSE);
	}

	void PxeWindow::initializeGl()
	{
		Impl& imp = impl();
		std::lock_guard lock(imp._windowMutex);
		PxeEngine& engine = PxeEngine::getInstance();
		imp._sdlWindow = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, imp._width, imp._height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		if (!imp._sdlWindow) {
			PXE_CHECKSDLERROR();
			PXE_ERROR("Failed to create PxeWindow's SDL_Window");
			setErrorStatus();
			setShouldClose();
			return;
		}

		if (engine.createdPrimaryGlContext()) {
			imp._renderTexture = new(std::nothrow) PxeRenderTexture(imp._width, imp._height, true);
			if (!imp._renderTexture) {
				PXE_ERROR("Failed to create PxeWindow's PxeRenderTexture");
				setErrorStatus();
				setShouldClose();
				return;
			}

			engine.forceInitializeGlAsset(*imp._renderTexture);
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
			PXE_CHECKSDLERROR();
		}
		else {
			SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
			PXE_CHECKSDLERROR();
		}

		imp._sdlGlContext = SDL_GL_CreateContext(imp._sdlWindow);
		if (!imp._sdlGlContext) {
			PXE_CHECKSDLERROR();
			PXE_ERROR("Failed to create PxeWindow's OpenGl context");
			setErrorStatus();
			setShouldClose();
			return;
		}

		engine.initGlContext();
		// TODO Can this fail gracefully?
		imp._guiContext = ImGui::CreateContext(engine.getRenderPipeline().getFontAtlas());
		//if (!_guiContext) {
		//	PXE_ERROR("Failed to create window GUI context");
		//	setErrorStatus();
		//	setShouldClose();
		//	return;
		//}

		ImGui::SetCurrentContext(imp._guiContext);
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		//ImGui::StyleColorsLight();

		// TODO Can this fail gracefully?
		// OpenGl context is not needed here as this is not the ImGui docking branch
		ImGui_ImplSDL2_InitForOpenGL(imp._sdlWindow, nullptr);
		//if (!ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, nullptr)) {
		//	PXE_ERROR("Failed to create window GUI context");
		//	setErrorStatus();
		//	setShouldClose();
		//	return;
		//}

		PxeGlImGuiBackend* backend = engine.getRenderPipeline().getGuiBackend();
		backend->grab();
		backend->installBackend();
		// TODO De-duplicate this block
		if (!engine.createdPrimaryGlContext()) {

			imp._renderTexture = new(std::nothrow) PxeRenderTexture(imp._width, imp._height, true);
			if (!imp._renderTexture) {
				PXE_ERROR("Failed to create PxeWindow's PxeRenderTexture");
				setErrorStatus();
				setShouldClose();
				return;
			}

			// We assume this context will become the primary OpenGl context
			engine.forceInitializeGlAsset(*imp._renderTexture);
		}

		if (imp._renderTexture->getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
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

		imp._vsyncMode = engine.getRenderPipeline().getVSyncMode();
		setFlag(SWAP_CHANGED);
		engine.registerWindow(*this);
		engine.bindPrimaryGlContext();
	}

	void PxeWindow::uninitializeGl()
	{
		Impl& imp = impl();
		PxeEngine& engine = PxeEngine::getInstance();
		engine.unregisterWindow(*this);
		bindGlContext();
		deleteFramebuffer();
		engine.bindPrimaryGlContext();
		if (imp._renderTexture) {
			imp._renderTexture->drop();
			imp._renderTexture = nullptr;
		}

		bindGuiContext();
		PxeGlImGuiBackend* backend = static_cast<PxeGlImGuiBackend*>(ImGui::GetIO().BackendRendererUserData);
		backend->uninstallBackend();
		backend->drop();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext(imp._guiContext);
		imp._guiContext = nullptr;

		if (imp._sdlGlContext) {
			SDL_GL_DeleteContext(imp._sdlGlContext);
			imp._sdlGlContext = nullptr;
		}

		if (imp._sdlWindow) {
			SDL_DestroyWindow(imp._sdlWindow);
			imp._sdlWindow = nullptr;
		}
	}

	bool PxeWindow::getFlag(WindowFlag flag) const
	{
		return impl()._flags & (uint8_t)flag;
	}

	void PxeWindow::clearFlag(WindowFlag flag)
	{
		impl()._flags &= ~(uint8_t)flag;
	}

	void PxeWindow::setFlag(WindowFlag flag, bool value)
	{
		Impl& imp = impl();
		imp._flags &= ~(uint8_t)flag;
		if (value)
			imp._flags |= (uint8_t)flag;
	}

	void PxeWindow::setFlag(WindowFlag flag)
	{
		impl()._flags |= (uint8_t)flag;
	}

	void PxeWindow::deleteFramebuffer()
	{
		Impl& imp = impl();
		glDeleteFramebuffers(1, &imp._internalFramebuffer);
		imp._internalFramebuffer = 0;
	}

	bool PxeWindow::createFramebuffer()
	{
		Impl& imp = impl();
		if (imp._internalFramebuffer != 0) {
			PXE_WARN("Called create framebuffer with existing framebuffer, deleting old framebuffer");
			deleteFramebuffer();
		}

		if (!imp._renderTexture || imp._renderTexture->getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_ERROR("Failed to create PxeWindow's glFramebuffer, invalid PxeRenderTexture");
			return false;
		}

		glGenFramebuffers(1, &imp._internalFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, imp._internalFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, imp._renderTexture->getGlTextureId(), 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			deleteFramebuffer();
			PXE_ERROR("Failed to create PxeWindow's glFramebuffer, Framebuffer status: " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
			return false;
		}

		return true;
	}

	PXE_NODISCARD SDL_GLContext PxeWindow::getGlContext() const
	{
		return impl()._sdlGlContext;
	}

	void PxeWindow::setGlContext(SDL_GLContext ctx)
	{
		impl()._sdlGlContext = ctx;
	}

	void PxeWindow::setSDLWindow(SDL_Window* window)
	{
		impl()._sdlWindow = window;
	}

	void PxeWindow::bindGuiContext()
	{
		Impl& imp = impl();
#ifdef PXE_DEBUG
		if (!imp._guiContext) {
			PXE_WARN("Binding invalid GUI context");
		}
#endif // PXE_DEBUG

		ImGui::SetCurrentContext(imp._guiContext);
	}

	void PxeWindow::prepareForRender()
	{
		Impl& imp = impl();
		PxeEngine& engine = PxeEngine::getInstance();
		std::unique_lock lock(imp._windowMutex);
		if (!imp._sdlWindow) return;
		if (getFlag(TITLE_CHANGED)) {
			SDL_SetWindowTitle(imp._sdlWindow, imp._title);
			clearFlag(TITLE_CHANGED);
		}

		if (getFlag(SIZE_CHANGED)) {
			SDL_SetWindowSize(imp._sdlWindow, imp._width, imp._height);
			clearFlag(SIZE_CHANGED);
		}

		SDL_GetWindowSizeInPixels(imp._sdlWindow, &imp._width, &imp._height);
	}

	void PxeWindow::bindGlContext()
	{
		Impl& imp = impl();
		if (imp._sdlWindow && imp._sdlGlContext)
			SDL_GL_MakeCurrent(imp._sdlWindow, imp._sdlGlContext);
	}

	void PxeWindow::setVsyncMode(int8_t mode)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowMutex);
		if (imp._vsyncMode != mode) {
			imp._vsyncMode = mode;
			setFlag(SWAP_CHANGED);
		}
	}

	void PxeWindow::acquireReadLock()
	{
		impl()._windowMutex.lock_shared();
	}

	void PxeWindow::releaseReadLock()
	{
		impl()._windowMutex.unlock_shared();
	}

	void PxeWindow::swapFramebuffers()
	{
		Impl& imp = impl();
		PxeEngine& engine = PxeEngine::getInstance();
		setVsyncMode(engine.getRenderPipeline().getVSyncMode());
		std::unique_lock lock(imp._windowMutex);
		if (getFlag(SIZE_CHANGED)) {
			PXE_WARN("PxeWindow size changed during frame render, skipping frame");
			return;
		}

		bindGlContext();
		if (getFlag(SWAP_CHANGED)) {
			SDL_GL_SetSwapInterval(imp._vsyncMode);
			clearFlag(SWAP_CHANGED);
		}

		glBindFramebuffer(GL_READ_FRAMEBUFFER, imp._internalFramebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, imp._width, imp._height);
		glDisable(GL_SCISSOR_TEST);
		glBlitFramebuffer(0, 0, imp._width, imp._height, 0, 0, imp._width, imp._height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		SDL_GL_SwapWindow(imp._sdlWindow);
		engine.bindPrimaryGlContext();
	}

	void PxeWindow::setScene(PxeScene* scene)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowMutex);
		if (imp._scene) imp._scene->drop();
		if ((imp._scene = scene)) imp._scene->grab();
	}

	PxeScene* PxeWindow::getScene() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._windowMutex);
		return imp._scene;
	}

	PXE_NODISCARD int32_t PxeWindow::getWindowWidth() const
	{
		return impl()._width;
	}

	PXE_NODISCARD int32_t PxeWindow::getWindowHeight() const
	{
		return impl()._height;
	}

	void PxeWindow::setWindowSize(int32_t width, int32_t height)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowMutex);
		if (imp._width != width || imp._height != height) {
			imp._width = width;
			imp._height = height;
			setFlag(SIZE_CHANGED);
		}
	}

	PXE_NODISCARD const char* PxeWindow::getWindowTitle() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._windowMutex);
		return imp._title;
	}

	void PxeWindow::setWindowTitle(const char* title)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowMutex);
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
		if (imp._title)
			free(imp._title);
		imp._title = newTitle;
		setFlag(TITLE_CHANGED);
	}

	void PxeWindow::setCamera(PxeCamera* camera)
	{
		Impl& imp = impl();
		std::unique_lock lock(imp._windowMutex);
		if (imp._camera) imp._camera->drop();
		if ((imp._camera = camera)) imp._camera->grab();
	}

	PXE_NODISCARD PxeCamera* PxeWindow::getCamera() const
	{
		const Impl& imp = impl();
		std::shared_lock lock(imp._windowMutex);
		return imp._camera;
	}

	PXE_NODISCARD PxeCamera* PxeWindow::getRenderCamera() const
	{
		return impl()._camera;
	}

	PXE_NODISCARD PxeRenderTexture* PxeWindow::getRenderOutputTexture() const
	{
		return impl()._renderTexture;
	}

	PXE_NODISCARD PxeScene* PxeWindow::getRenderScene() const
	{
		return impl()._scene;
	}

	PXE_NODISCARD int32_t PxeWindow::getRenderWidth() const
	{
		return impl()._width;
	}

	PXE_NODISCARD int32_t PxeWindow::getRenderHeight() const
	{
		return impl()._height;
	}

	uint32_t PxeWindow::getSDLWindowId() const
	{
		const Impl& imp = impl();
		if (!imp._sdlWindow) {
			PXE_ERROR("Attempted to get id of invalid SDL window");
			return static_cast<uint32_t>(-1);
		}

		return SDL_GetWindowID(imp._sdlWindow);
	}

	PXE_NODISCARD PxeRenderTexture* PxeWindow::getRenderTexture() const
	{
		return impl()._renderTexture;
	}
}