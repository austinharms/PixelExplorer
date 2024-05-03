#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_MESH_H_
#define PE_GL_GRAPHICS_GL_MESH_H_
#include "PE_gl_defines.h"
#include "PE_gl_types.h"
#include "PE_memory.h"
#include "PE_log.h"
#include "PE_errors.h"
#include "glad/gl.h"

namespace pecore::graphics::open_gl {
	struct PE_RenderMesh {
	public:
		enum {
			BUF_VERTEX = 0,
			BUF_ELEMENT = 1,
		};

		GLuint buffers[2];

		static PE_RenderMesh* CreateRenderMesh(GlLoadWorker& worker, GlContextWrapper& ctx, void* vertices, size_t vertices_size, void* indices, size_t indices_size) PE_NOEXCEPT {
			PE_RenderMesh* mesh = static_cast<PE_RenderMesh*>(PE_malloc(sizeof(PE_RenderMesh)));
			if (!mesh) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate PE_RenderMesh"));
				return nullptr;
			}

			int return_code;
			worker.PushBlockingTWork<UploadData>(mesh, ctx, vertices, vertices_size, indices, indices_size);
			if (return_code != PE_ERROR_NONE) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to copy mesh to PE_RenderMesh"));
				PE_free(mesh);
				return nullptr;
			}

			return mesh;
		}

		// Note: must be called from an OpenGl thread
		void UploadData(GlContextWrapper& ctx, void* vertices, size_t vertices_size, void* indices, size_t indices_size, int& return_code) {
			GladGLContext& gl = ctx.gl;
			buffers[BUF_VERTEX] = 0;
			buffers[BUF_ELEMENT] = 0;
			gl.GenBuffers(PE_ARRAY_LEN(buffers), buffers);
			for (int i = 0; i < PE_ARRAY_LEN(buffers); ++i) {
				if (buffers[i] == 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create OpenGL buffers for PE_RenderMesh"));
					return_code = PE_ERROR_GENERAL;
					return;
				}
			}

			gl.BindBuffer(GL_ARRAY_BUFFER, buffers[PE_RenderMesh::BUF_VERTEX]);
			// TDOD Check if data was uploaded
			gl.BufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

			gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[PE_RenderMesh::BUF_ELEMENT]);
			// TDOO Check if data was uploaded
			gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
			return_code = PE_ERROR_NONE;
		}

		void DestroyMesh(GlLoadWorker& worker, GlContextWrapper& ctx) {
			worker.PushBlockingTWork<DeleteBuffers>(this, ctx);
			PE_free(this);
		}

		// Note: this must be called from an OpenGl thread
		void DeleteBuffers(GlContextWrapper& ctx) {
			ctx.gl.DeleteBuffers(PE_ARRAY_LEN(buffers), buffers);
		}
	};
}
#endif // !PE_GL_GRAPHICS_GL_MESH_H_
#endif // !PE_DISABLE_OPENGL
