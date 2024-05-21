#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_WINDOW_H_
#define PE_GL_GRAPHICS_GL_WINDOW_H_
#include "PE_defines.h"
#include "PE_graphics.h"
#include "PE_opengl_common.h"
#include "SDL_video.h"

namespace pe::internal::opengl {
	class GlWindow : public Window {
	public:
		static GlWindow* Create(const char* title, int width, int height, Uint32 flags) PE_NOEXCEPT;
		void OnDrop() PE_NOEXCEPT PE_OVERRIDE;
		PE_NODISCARD SDL_Window* GetSDLHandle() const PE_OVERRIDE;

	protected:
		SDL_Window* window_handle;
#if PE_GL_CONTEXT_PER_WINDOW
		GlContextWrapper context;
		ThreadWorker worker;
#endif

		// Must be run on the main/event thread
		void CreateWindowHandle(const char* title, int width, int height, Uint32 flags) PE_NOEXCEPT;
		// Must be run on the main/event thread
		void DestroyWindowHandle();
#if PE_GL_CONTEXT_PER_WINDOW
		void DestroyContext();
		void SetContextActive(int& return_code);
#endif
	};
}
#endif // !PE_GL_GRAPHICS_GL_WINDOW_H_
#endif // !PE_DISABLE_OPENGL
