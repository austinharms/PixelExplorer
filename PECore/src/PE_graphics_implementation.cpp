#include "PE_graphics_implementation.h"
#include "PE_global.h"
#include "PE_exception.h"

namespace pe::internal {
	PE_NODISCARD GraphicsImplementation& GraphicsImplementation::GetGraphicsImplementation()
	{
		return GetGlobalData().graphics;
	}

	GraphicsImplementation::GraphicsImplementation() : 
		window_(nullptr),
		init_(false) {
	}

	GraphicsImplementation::~GraphicsImplementation()
	{
	}

	void GraphicsImplementation::Init()
	{
		if (init_) {
			throw ErrorCodeException(PE_ERROR_ALREADY_INITIALIZED);
		}

		ErrorCode err_code = PE_ERROR_NONE;
		if ((err_code = load_worker_.Start()) != PE_ERROR_NONE) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to start background load thread"));
			return err_code;
		}

		window_ = SDL_CreateWindow(PE_TEXT("PixelExplorer"), 600, 400, SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);
		if (!window_) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
			SDL_ClearError();
			return PE_ERROR_GENERAL;
		}

		if (SDL_SetWindowPosition(window_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED) != 0) {
			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window position: %s"), SDL_GetError());
			SDL_ClearError();
		}

		load_context_ = SDL_GL_CreateContext(window_);
		if (load_context_ == nullptr) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window OpenGL load context: %s"), SDL_GetError());
			SDL_ClearError();
			SDL_DestroyWindow(window_);
			return PE_ERROR_GENERAL;
		}

		main_context_ = SDL_GL_CreateContext(window_);
		if (main_context_ == nullptr) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window OpenGL render context: %s"), SDL_GetError());
			SDL_ClearError();
			SDL_GL_DeleteContext(load_context_);
			load_context_ = nullptr;
			SDL_DestroyWindow(window_);
			window_ = nullptr;
			return PE_ERROR_GENERAL;
		}



		return PE_ERROR_NONE;
	}

	void GraphicsImplementation::Quit() PE_NOEXCEPT
	{
	}

	PE_NODISCARD graphics::Shader* GraphicsImplementation::LoadShader(const char* name, ErrorCode* err_out)
	{
		return nullptr;
	}

	PE_NODISCARD graphics::RenderMesh* GraphicsImplementation::CreateRenderMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size, ErrorCode* err_out)
	{
		return nullptr;
	}

	PE_NODISCARD void* GraphicsImplementation::GetSDLWindowHandle()
	{
		return static_cast<void*>(window_);
	}
}
