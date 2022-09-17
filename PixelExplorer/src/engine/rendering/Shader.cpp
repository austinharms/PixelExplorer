#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "common/Logger.h"
#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "RenderWindow.h"

namespace pixelexplorer::engine::rendering {
	// defaults to -10000 priority (pre onUpdate)
	Shader::Shader(const std::string& path) : GLObject(-10000), _path(path), _glId(0) {}

	Shader::~Shader() { Logger::debug(__FUNCTION__" Shader unloaded: " + _path); }

	uint32_t Shader::compileShader(uint32_t shaderType, const std::string& source) const
	{
		uint32_t id = glCreateShader(shaderType);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int32_t res;
		glGetShaderiv(id, GL_COMPILE_STATUS, &res);
		if (res == GL_FALSE) {
			int32_t errorLength;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &errorLength);
			char* errorString = (char*)malloc(errorLength);
			glGetShaderInfoLog(id, errorLength, &errorLength, errorString);
			if (shaderType == GL_VERTEX_SHADER) {
				Logger::error(std::string(__FUNCTION__" Failed to compile Vertex shader, ") + errorString);
			}
			else if (shaderType == GL_FRAGMENT_SHADER) {
				Logger::error(std::string(__FUNCTION__" Failed to compile Fragment shader, ") + errorString);
			}
			else {
				Logger::error(std::string(__FUNCTION__" Failed to compile shader, ") + errorString);
			}

			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	uint32_t Shader::loadShader(const std::string& path) const
	{
		enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
		std::ifstream stream(path);
		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;
		while (getline(stream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
				else {
					Logger::error(__FUNCTION__" Found unknow shader type '" + line + "' when loading shader from " + path);
					return 0;
				}
			}
			else {
				ss[(int32_t)type] << line << "\n";
			}
		}

		if (type == ShaderType::NONE) {
			Logger::error(__FUNCTION__" Failed to Load Shader, File Empty or Not Found, Path " + path);
			return 0;
		}

		uint32_t vs = compileShader(GL_VERTEX_SHADER, ss[(int)ShaderType::VERTEX].str());
		uint32_t fs = compileShader(GL_FRAGMENT_SHADER, ss[(int)ShaderType::FRAGMENT].str());

		uint32_t program = 0;
		if (vs != 0 && fs != 0) {
			program = glCreateProgram();
			glAttachShader(program, vs);
			glAttachShader(program, fs);
			glLinkProgram(program);
			glValidateProgram(program);
		}

		glDeleteShader(vs);
		glDeleteShader(fs);
		if (program != 0) Logger::debug(__FUNCTION__" Loaded Shader: " + path);
		return program;
	}

	int32_t Shader::getUniformLocation(const std::string& name)
	{
		auto uniform = _uniforms.find(name);
		if (uniform != _uniforms.end()) {
			return uniform->second;
		}
		else {
			int32_t u = glGetUniformLocation(_glId, name.c_str());
			_uniforms.insert({ name, u });
			if (u == -1)
				Logger::warn(__FUNCTION__" Failed to find uniform " + name + " in shader " + _path);

			return u;
		}
	}

	void Shader::setUniform1i(const std::string& name, int32_t value)
	{
		int32_t location = getUniformLocation(name);
		if (location == -1) return;
		glUniform1i(location, value);
	}

	void Shader::setUniform1f(const std::string& name, float value)
	{
		int32_t location = getUniformLocation(name);
		if (location == -1) return;
		glUniform1f(location, value);
	}

	void Shader::setUniform2fv(const std::string& name, const glm::vec2& value)
	{
		int32_t location = getUniformLocation(name);
		if (location == -1) return;
		glUniform2fv(location, 1, glm::value_ptr(value));
	}

	void Shader::setUniform3fv(const std::string& name, const glm::vec3& value)
	{
		int32_t location = getUniformLocation(name);
		if (location == -1) return;
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void Shader::setUniform4fv(const std::string& name, const glm::vec4& value)
	{
		int32_t location = getUniformLocation(name);
		if (location == -1) return;
		glUniform4fv(location, 1, glm::value_ptr(value));
	}

	void Shader::setUniformm4fv(const std::string& name, const glm::mat4& value)
	{
		int32_t location = getUniformLocation(name);
		if (location == -1) return;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::onInitialize()
	{
		_glId = loadShader(_path);
	}

	void Shader::onUpdate() {
		getRenderWindow()->setActiveShader(*this);
	}

	void Shader::onTerminate()
	{
		RenderWindow* window = getRenderWindow();
		window->removeShaderFromCache(*this);
		window->clearActiveShader(*this);
		glDeleteProgram(_glId);
		_glId = 0;
	}

	void Shader::bind() const
	{
		assert(isValid());
		glUseProgram(_glId);
	}

	void Shader::unbind() const
	{
		glUseProgram(0);
	}
}