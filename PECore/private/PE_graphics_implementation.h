#ifndef PE_GRAPHICS_IMPLEMENTATION_H_
#define PE_GRAPHICS_IMPLEMENTATION_H_
#include "PE_defines.h"
#include "PE_graphics.h"
#include "PE_errors.h"
#include "PE_worker_thread.h"
#include "SDL_video.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>

namespace pe::internal {
	class RenderScene
	{
	public:
		RenderScene();
		~RenderScene();

	private:
		std::vector<std::pair<graphics::Shader*, std::vector<std::pair<graphics::RenderMaterial*, std::vector<std::pair<graphics::GraphicsMesh*, std::vector<graphics::RenderMesh*>>>>>>> scene_tree_;
	};

	class GraphicsImplementation {
	public:
		PE_NODISCARD static GraphicsImplementation& GetGraphicsImplementation();

		void Init();
		void Quit() PE_NOEXCEPT;
		PE_NODISCARD graphics::Shader& LoadShader(const char* name);
		PE_NODISCARD graphics::GraphicsMesh& CreateRenderMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size);
		// Returns the SDL_Window struct pointer
		PE_NODISCARD void* GetSDLWindowHandle();
		GraphicsImplementation();
		~GraphicsImplementation();
		PE_NOCOPY(GraphicsImplementation);

	protected:
		std::unordered_map<std::string, graphics::Shader> shader_cache_;
		std::mutex shader_cache_mutex_;
		SDL_Window* window_;
		SDL_GLContext main_context_;
		SDL_GLContext load_context_;
		ThreadWorker load_worker_;
		bool init_;
	};
}
#endif // !PE_GRAPHICS_IMPLEMENTATION_H_
