#include "opengl_graphics/PE_opengl_shader.h"
#include "PE_asset_manager.h"
#include "PE_errors.h"
#include "PE_log.h"
#include <fstream>

namespace pe::internal::opengl {
	GlShader::GlShader(GlShaderCache& parent) :
		parent_cache(parent),
		cache_key(nullptr),
		program(0) {
	}

	void GlShader::OnDrop() {
		parent_cache.EraseShaderFromCache(*this);
	}

	graphics::Shader* GlShaderCache::GetShader(const char* name, ErrorCode* err_out) {
		try {
			std::string name_string(name);

		}
		catch (const std::bad_alloc& e) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get shader, %s"), e.what());
			if (err_out) {
				*err_out = PE_ERROR_OUT_OF_MEMORY;
			}
		}
		catch (const std::exception& e) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get shader, %s"), e.what());
			if (err_out) {
				*err_out = PE_ERROR_GENERAL;
			}
		}
		catch (...) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get shader, Unknown error occurred"));
			if (err_out) {
				*err_out = PE_ERROR_GENERAL;
			}
		}

		return nullptr;
	}

	GlShader* GlShaderCache::GetOrCreateShaderObjectUnsafe(const char* name) PE_EXCEPT {
		auto cached_value = shader_cache_.insert({ name, GlShader(*this) });
		GlShader& shader = cached_value.first->second;
		shader.cache_key = &(cached_value.first->first);
		return &shader;
	}

	void GlShaderCache::LoadShaderSource(const std::filesystem::path& path, ShaderSource& source_out) PE_EXCEPT {
		PE_STATIC_ASSERT(ShaderSource::SHADER_SOURCE_COUNT == 2, PE_TEXT("SHADER_SOURCE_ENUM_COUNT changed, Update GlShaderCache::LoadShaderSource"));
		ErrorCode err_code;
		TextAsset* shader_file = TextAsset::LoadFile(path, &err_code);
		if (err_code != PE_ERROR_NONE) {
			throw err_code;
		}

		char line_buffer[256];
		unsigned int cur_shader = ShaderSource::SHADER_SOURCE_COUNT;
		constexpr char* kVertexFileHeader = PE_TEXT("#shader vertex");
		const size_t kVertexFileHeaderLength = SDL_utf8strlen(kVertexFileHeader);
		constexpr char* kFragmentFileHeader = PE_TEXT("#shader fragment");
		const size_t kFragmentFileHeaderLength = SDL_utf8strlen(kFragmentFileHeader);
		ErrorCode code;
		while ((code = shader_file->GetLine(line_buffer, PE_ARRAY_LEN(line_buffer))) == PE_ERROR_NONE) {
			if (SDL_strncmp(line_buffer, kVertexFileHeader, kVertexFileHeaderLength) == 0) {
				cur_shader = ShaderSource::VERTEX_SOURCE;
			}
			else if (SDL_strncmp(line_buffer, kFragmentFileHeader, kFragmentFileHeaderLength) == 0) {
				cur_shader = ShaderSource::FRAGMENT_SOURCE;
			}
			else if (cur_shader >= ShaderSource::SHADER_SOURCE_COUNT) {
				throw std::exception(PE_TEXT("Failed to parse OpenGL shader file"));
			}
			else {
				source_out.source_code[cur_shader].append(line_buffer);
			}
		}

		if (code != PE_ERROR_EMPTY) {
			throw code;
		}
	}

	void GlShaderCache::CompileShaderSource(GladGLContext& gl, GlShader& shader, const ShaderSource& source)
	{
		PE_STATIC_ASSERT(ShaderSource::SHADER_SOURCE_COUNT == 2, PE_TEXT("SHADER_SOURCE_ENUM_COUNT Changed update GlShaderCache::CompileShaderSource"));
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

	void GlShaderCache::DeleteShaderProgram(GladGLContext& gl, GlShader& shader) {
		if (shader.program == 0) {
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Called delete program when program was invalid"));
		}

		gl.DeleteProgram(shader.program);
		shader.program = 0;
	}
}