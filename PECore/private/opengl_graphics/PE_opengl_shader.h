#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_SHADER_H_
#define PE_GL_GRAPHICS_GL_SHADER_H_
#include "PE_defines.h"
#include "PE_opengl_common.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <filesystem>

namespace pe::internal::opengl {
	class GlShaderCache;
	class GlShader : public graphics::Shader {
	public:
		const std::string* cache_key;
		GlShaderCache& parent_cache;
		GLuint program;

		GlShader(GlShaderCache& parent);
		void OnDrop() PE_OVERRIDE;
	};

	class GlShaderCache {
	public:
		PE_NODISCARD graphics::Shader* GetShader(const char* name, ErrorCode* err_out) PE_NOEXCEPT;
		// Note: this does not lock the shader cache mutex
		GlShader* GetOrCreateShaderObjectUnsafe(const char* name) PE_EXCEPT;

	private:
		friend class GlShader;
		struct ShaderSource {
			enum {
				FRAGMENT_SOURCE = 0,
				VERTEX_SOURCE,
				SHADER_SOURCE_COUNT
			};

			std::string source_code[SHADER_SOURCE_COUNT];
		};

		std::unordered_map<std::string, GlShader> shader_cache_;
		std::mutex shader_cache_mutex_;

		void EraseShaderFromCache(GlShader& shader);
		static void LoadShaderSource(const std::filesystem::path& path, ShaderSource& source_out) PE_EXCEPT;
		// Must be run on an OpenGL thread
		static void CompileShaderSource(GladGLContext& gl, GlShader& shader, const ShaderSource& source) PE_NOEXCEPT;
		// Must be run on an OpenGL thread
		static void DeleteShaderProgram(GladGLContext& gl, GlShader& shader);
	};
}
#endif // !PE_GL_GRAPHICS_GL_SHADER_H_
#endif // !PE_DISABLE_OPENGL
