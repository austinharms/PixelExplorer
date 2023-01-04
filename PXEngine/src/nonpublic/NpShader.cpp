#include "NpShader.h"

#include <fstream>
#include <string>
#include <sstream>

#include "GL/glew.h"
#include "NpLogger.h"
#include "glm/gtc/type_ptr.hpp"

namespace pxengine {
	namespace nonpublic {
		NpShader::NpShader(const std::string& path) : _path(path) {
			_glProgramId = 0;
		}

		NpShader::~NpShader()
		{
			PXE_INFO("Destroyed PxeShader: " + _path);
		}

		bool NpShader::validateProgram()
		{
			if (_glProgramId == 0) {
				PXE_ERROR("Attempted to validate program of invalid PxeShader");
				return false;
			}

			int32_t res;
			glValidateProgram(_glProgramId);
			glGetProgramiv(_glProgramId, GL_VALIDATE_STATUS, &res);
			if (res == GL_FALSE) {
				int32_t errorLength;
				glGetProgramiv(_glProgramId, GL_INFO_LOG_LENGTH, &errorLength);
				char* errorString = (char*)alloca(errorLength);
				glGetProgramInfoLog(_glProgramId, errorLength, NULL, errorString);
				PXE_ERROR("Failed to validate shader " + _path + " " + errorString);
				setErrorStatus();
			}

			return res;
		}

		bool NpShader::getValid() const
		{
			return _glProgramId;
		}

		void NpShader::setUniform1i(const std::string& name, const int32_t value)
		{
			glUniform1i(getUniformLocation(name), value);
		}

		void NpShader::setUniform1f(const std::string& name, const float value)
		{
			glUniform1f(getUniformLocation(name), value);
		}

		void NpShader::setUniform2fv(const std::string& name, const glm::vec2& value)
		{
			glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
		}

		void NpShader::setUniform3fv(const std::string& name, const glm::vec3& value)
		{
			glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
		}

		void NpShader::setUniform4fv(const std::string& name, const glm::vec4& value)
		{
			glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
		}

		void NpShader::setUniformm4fv(const std::string& name, const glm::mat4& value)
		{
			glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
		}

		void NpShader::initializeGl()
		{
			_uniformLocations.clear();
			_glProgramId = loadShaderFile(_path);
			if (getValid()) {
				PXE_INFO("Initialized PxeShader " + _path);
			}
			else {
				setErrorStatus();
			}
		}

		void NpShader::uninitializeGl()
		{
			if (getValid()) {
				glDeleteProgram(_glProgramId);
				_glProgramId = 0;
				PXE_INFO("Uninitialized PxeShader " + _path);
			}
		}

		void NpShader::bind()
		{
			if (!getValid()) {
				PXE_WARN("Attempted to bind invalid PxeShader");
				return;
			}

			glUseProgram(_glProgramId);
		}

		void NpShader::unbind()
		{
#ifdef PXE_DEBUG
			uint32_t previousProgram;
			glGetIntegerv(GL_CURRENT_PROGRAM, (int32_t*)(&previousProgram));
			if (previousProgram != _glProgramId) {
				PXE_WARN("unbind called on unbound PxeShader");
			}
#endif // PXE_DEBUG

			glUseProgram(0);
		}

		uint32_t NpShader::compileShader(PxeShaderType shaderType, const std::string& source)
		{
			if (shaderType == PxeShaderType::NONE) {
				PXE_ERROR("Attempted to compile NONE type shader");
				return 0;
			}

			uint32_t id = glCreateShader(getShaderGlenum(shaderType));
			const char* src = source.c_str();
			glShaderSource(id, 1, &src, nullptr);
			glCompileShader(id);

			int32_t res;
			glGetShaderiv(id, GL_COMPILE_STATUS, &res);
			if (res == GL_FALSE) {
				int32_t errorLength;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &errorLength);
				char* errorString = (char*)alloca(errorLength);
				glGetShaderInfoLog(id, errorLength, NULL, errorString);
				PXE_ERROR(std::string("Failed to compile ") + getShaderName(shaderType) + " shader: " + errorString);
				glDeleteShader(id);
				return 0;
			}
#ifdef PXE_DEBUG
			else {
				int32_t msgLength;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &msgLength);
				char* msgString = (char*)alloca(msgLength);
				glGetShaderInfoLog(id, msgLength, NULL, msgString);
				PXE_INFO(std::string("Compiled ") + getShaderName(shaderType) + " shader: " + msgString);
			}
#endif
			return id;
		}

		uint32_t NpShader::loadShaderFile(const std::string& path)
		{
			std::ifstream stream(path);
			std::string line;
			std::stringstream ss[(int32_t)PxeShaderType::SHADER_COUNT];
			PxeShaderType type = PxeShaderType::NONE;
			while (getline(stream, line)) {
				if (line.find("#shader") != std::string::npos) {
					if (line.find("vertex") != std::string::npos) {
						type = PxeShaderType::VERTEX;
					}
					else if (line.find("fragment") != std::string::npos) {
						type = PxeShaderType::FRAGMENT;
					}
					else if (line.find("geometry") != std::string::npos) {
						type = PxeShaderType::GEOMETRY;
					}
					else {
						PXE_ERROR("Failed to load PxeShader " + path + " found unknown shader type \"" + line + "\"");
						return;
					}
				}
				else {
					ss[(int32_t)type] << line << "\n";
				}
			}

			stream.close();
			if (type == PxeShaderType::NONE) {
				PXE_ERROR("Failed to load PxeShader " + path + " file empty or not found");
				return;
			}

			uint32_t programId = glCreateProgram();
			if (programId == 0) {
				PXE_ERROR("Failed to load PxeShader " + path + +" failed to create Gl Program");
				return 0;
			}

			bool shaderError = false;
			for (int32_t i = 0; i < (int32_t)PxeShaderType::SHADER_COUNT; ++i) {
				if (ss[i].rdbuf()->in_avail() == 0) continue;
				uint32_t shader = compileShader((PxeShaderType)i, ss[i].str());
				if (!shader) {
					shaderError = true;
					break;
				}

				glAttachShader(programId, shader);
				// this should not delete the shader until the program is deleted or
				// the shader is detached, so it is safe to call now
				glDeleteShader(shader);
			}

			if (shaderError) {
				glDeleteProgram(programId);
				PXE_ERROR("Failed to load PxeShader " + path + +" failed to compile all shaders");
				return 0;
			}

			glLinkProgram(programId);
			int32_t res;
			glGetProgramiv(programId, GL_LINK_STATUS, &res);
			if (res == GL_FALSE) {
				int32_t errorLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &errorLength);
				char* errorString = (char*)alloca(errorLength);
				glGetProgramInfoLog(programId, errorLength, NULL, errorString);
				PXE_ERROR("Failed to link shader " + path + " " + errorString);
				glDeleteProgram(programId);
				return 0;
			}
#ifdef PXE_DEBUG
			else {
				int32_t msgLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &msgLength);
				char* msgString = (char*)alloca(msgLength);
				glGetProgramInfoLog(programId, msgLength, NULL, msgString);
				PXE_INFO("Linked shader " + path + " " + msgString);
			}
#endif

			glValidateProgram(programId);
			glGetProgramiv(programId, GL_VALIDATE_STATUS, &res);
			if (res == GL_FALSE) {
				int32_t errorLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &errorLength);
				char* errorString = (char*)alloca(errorLength);
				glGetProgramInfoLog(programId, errorLength, NULL, errorString);
				PXE_ERROR("Failed to validate shader " + path + " " + errorString);
				glDeleteProgram(programId);
				return 0;
			}
#ifdef PXE_DEBUG
			else {
				int32_t msgLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &msgLength);
				char* msgString = (char*)alloca(msgLength);
				glGetProgramInfoLog(programId, msgLength, NULL, msgString);
				PXE_INFO("Validated shader " + path + " " + msgString);
			}
#endif

			return programId;
		}

		constexpr const char* NpShader::getShaderName(const PxeShaderType type)
		{
			switch (type)
			{
			case PxeShaderType::FRAGMENT:
				return "FRAGMENT";
			case PxeShaderType::VERTEX:
				return "VERTEX";
			case PxeShaderType::GEOMETRY:
				return "GEOMETRY";
			default:
				return "UNKNOWN";
			}
		}

		constexpr const GLenum NpShader::getShaderGlenum(const PxeShaderType type)
		{
			switch (type)
			{
			case PxeShaderType::FRAGMENT:
				return GL_FRAGMENT_SHADER;
			case PxeShaderType::VERTEX:
				return GL_VERTEX_SHADER;
			case PxeShaderType::GEOMETRY:
				return GL_GEOMETRY_SHADER;
			default:
				return GL_NONE;
			}
		}

		int32_t NpShader::getUniformLocation(const std::string& name)
		{
			if (!getValid()) {
				PXE_WARN("Attempted to get uniform location of invalid PxeShader");
				return -1;
			}

			auto uniform = _uniformLocations.find(name);
			if (uniform != _uniformLocations.end()) {
				return uniform->second;
			}
			else {
				int32_t u = glGetUniformLocation(_glProgramId, name.c_str());
				_uniformLocations.insert({ name, u });
				if (u == -1) {
					PXE_WARN("Failed to find uniform \"" + name + "\" in PxeShader " + _path);
				}

				return u;
			}
		}

		uint32_t NpShader::getGlProgramId() const
		{
			return _glProgramId;
		}

		const std::string& NpShader::getShaderPath() const
		{
			return _path;
		}
	}
}