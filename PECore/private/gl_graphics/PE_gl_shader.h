#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_SHADER_H_
#define PE_GL_GRAPHICS_GL_SHADER_H_
#include "PE_gl_defines.h"
#include "PE_gl_types.h"
#include "PE_errors.h"
#include "PE_log.h"
#include "PE_asset_manager_internal.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <filesystem>
#include <fstream>

namespace pecore::graphics::open_gl {
	struct PE_Shader {
		size_t ref_count;
		const std::string* cache_key;
		GLuint program;
		bool loaded;

		constexpr PE_Shader() :
			ref_count(0),
			cache_key(nullptr),
			program(0),
			loaded(false) {
		}
	};

	class ShaderCache PE_FINAL {
	public:
		struct ShaderSource {
			enum {
				INVLAID_SOURCE = -1,
				FRAGMENT_SOURCE = 0,
				VERTEX_SOURCE,
				SHADER_SOURCE_ENUM_COUNT
			};

			std::string source_code[SHADER_SOURCE_ENUM_COUNT];
		};

	private:
		std::unordered_map<std::string, PE_Shader> shader_cache;
		std::mutex shader_cache_mutex;

	public:
		// Note: this does not lock the shader cache mutex
		PE_Shader* GetOrCreateShaderUnsafe(const char* name) PE_EXCEPT {
			auto cached_value = shader_cache.insert({ name, PE_Shader() });
			PE_Shader& shader = cached_value.first->second;
			if (shader.loaded) {
				shader.ref_count += 1;
			}
			else {
				shader.cache_key = &(cached_value.first->first);
				shader.ref_count = 1;
			}

			return &shader;
		}

		static void LoadShaderSource(const std::filesystem::path& path, ShaderSource& source_out) PE_EXCEPT {
			PE_STATIC_ASSERT(ShaderSource::SHADER_SOURCE_ENUM_COUNT == 2, PE_TEXT("SHADER_SOURCE_ENUM_COUNT Changed update LoadShaderSource"));
			std::ifstream shader_stream;
			std::streampos shader_end;
			if (int err = PE_GetFileHandle(path, shader_stream, shader_end) != PE_ERROR_NONE) {
				throw static_cast<PE_ErrorCode>(err);
			}

			int cur_shader = ShaderSource::INVLAID_SOURCE;
			for (std::string line; shader_stream.tellg() < shader_end && std::getline(shader_stream, line);) {
				if (line.rfind("#shader vertex", 0) == 0) {
					cur_shader = ShaderSource::VERTEX_SOURCE;
				}
				else if (line.rfind("#shader fragment", 0) == 0) {
					cur_shader = ShaderSource::FRAGMENT_SOURCE;
				}
				else if (cur_shader <= ShaderSource::INVLAID_SOURCE || cur_shader >= ShaderSource::SHADER_SOURCE_ENUM_COUNT) {
					throw std::exception("Failed to parse OpenGL shader file");
				}
				else {
					source_out.source_code[cur_shader].append(line);
				}
			}
		}

		// Must be run on an opengl thread
		static void CompileShaderProgram(GladGLContext& gl, PE_Shader& shader, const ShaderSource& source)  PE_NOEXCEPT {
			PE_STATIC_ASSERT(ShaderSource::SHADER_SOURCE_ENUM_COUNT == 2, PE_TEXT("SHADER_SOURCE_ENUM_COUNT Changed update CompileShaderProgram"));
			shader.program = 0;
			GLint compiled;
			GLuint vert_shader = gl.CreateShader(GL_VERTEX_SHADER);
			if (vert_shader == 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create vertex shader"));
				return;
			}

			const char* vert_source = source.source_code[ShaderSource::VERTEX_SOURCE].c_str();
			gl.ShaderSource(vert_shader, 1, &vert_source, nullptr);
			gl.CompileShader(vert_shader);
			gl.GetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				GLchar message[256];
				gl.GetShaderInfoLog(vert_shader, 256, nullptr, message);
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile vertex shader, error: %s"), message);
				gl.DeleteProgram(vert_shader);
				return;
			}

			GLuint frag_shader = gl.CreateShader(GL_FRAGMENT_SHADER);
			if (frag_shader == 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create fragment shader"));
				gl.DeleteProgram(vert_shader);
				return;
			}

			const char* frag_source = source.source_code[ShaderSource::FRAGMENT_SOURCE].c_str();
			gl.ShaderSource(frag_shader, 1, &frag_source, nullptr);
			gl.CompileShader(frag_shader);
			gl.GetShaderiv(frag_shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				GLchar message[256];
				gl.GetShaderInfoLog(frag_shader, 256, nullptr, message);
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile fragment shader, error: %s"), message);
				gl.DeleteProgram(vert_shader);
				gl.DeleteProgram(frag_shader);
				return;
			}

			GLuint program = gl.CreateProgram();
			if (program != 0) {
				gl.AttachShader(program, vert_shader);
				gl.AttachShader(program, frag_shader);
				gl.LinkProgram(program);
				gl.GetProgramiv(program, GL_LINK_STATUS, &compiled);
				if (compiled != GL_TRUE)
				{
					GLsizei log_length = 0;
					GLchar message[256];
					gl.GetProgramInfoLog(program, 1024, &log_length, message);
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile shader, error: %s"), message);
					gl.DeleteProgram(program);
				}
				else {
					shader.program = program;
				}
			}

			gl.DeleteProgram(vert_shader);
			gl.DeleteProgram(frag_shader);
		}

		static PE_FORCEINLINE bool ReleaseShader(PE_Shader& shader) PE_NOEXCEPT {
			PE_DEBUG_ASSERT(shader.ref_count > 0, PE_TEXT("Released unreferenced PE_Shader"));
			return --shader.ref_count == 0;
		}

		// Must be run on an opengl thread
		static PE_FORCEINLINE void DeleteShaderProgram(GladGLContext& gl, PE_Shader& shader) PE_NOEXCEPT {
			if (shader.program == 0) {
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Called delete program when program was invalid"));
			}
			gl.DeleteProgram(shader.program);
			shader.program = 0;
		}

		// Note: this does not lock the shader cache mutex
		PE_FORCEINLINE void DestroyShaderUnsafe(PE_Shader& shader) PE_NOEXCEPT {
			PE_DEBUG_ASSERT(shader.ref_count == 0, PE_TEXT("Deleted referenced PE_Shader"));
			PE_DEBUG_ASSERT(shader.program == 0, PE_TEXT("Deleted PE_Shader with valid program"));
			shader_cache.erase(*shader.cache_key);
		}

		PE_FORCEINLINE void Lock() PE_EXCEPT {
			shader_cache_mutex.lock();
		}

		PE_FORCEINLINE void Unlock() PE_NOEXCEPT {
			shader_cache_mutex.unlock();
		}
	};
}
#endif // !PE_GL_GRAPHICS_GL_SHADER_H_
#endif // !PE_DISABLE_OPENGL
