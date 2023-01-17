#include "NpShader.h"

#include <fstream>
#include <string>
#include <sstream>

#include "GL/glew.h"
#include "NpLogger.h"
#include "glm/gtc/type_ptr.hpp"
#include "NpEngine.h"

namespace pxengine {
	namespace nonpublic {
		NpShader::NpShader(const std::filesystem::path& path) : _path(path) {
			_glProgramId = 0;
			_initializationCount = 0;
		}

		NpShader::~NpShader()
		{
			PXE_INFO("Destroyed PxeShader: " + _path.string());
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
				PXE_ERROR("Failed to validate shader " + _path.string() + " " + errorString);
				setErrorStatus();
			}

			return res;
		}

		bool NpShader::getValid() const
		{
			return _glProgramId;
		}

		void NpShader::setUniform1fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniform1fv(location, count, values);
		}

		void NpShader::setUniform2fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniform2fv(location, count, values);
		}

		void NpShader::setUniform3fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniform3fv(location, count, values);
		}

		void NpShader::setUniform4fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniform4fv(location, count, values);
		}

		void NpShader::setUniform1iv(const int32_t location, const int32_t* values, uint32_t count)
		{
			glUniform1iv(location, count, values);
		}

		void NpShader::setUniform2iv(const int32_t location, const int32_t* values, uint32_t count)
		{
			glUniform2iv(location, count, values);
		}

		void NpShader::setUniform3iv(const int32_t location, const int32_t* values, uint32_t count)
		{
			glUniform3iv(location, count, values);
		}

		void NpShader::setUniform4iv(const int32_t location, const int32_t* values, uint32_t count)
		{
			glUniform4iv(location, count, values);
		}

		void NpShader::setUniform1uiv(const int32_t location, const uint32_t* values, uint32_t count)
		{
			glUniform1uiv(location, count, values);
		}

		void NpShader::setUniform2uiv(const int32_t location, const uint32_t* values, uint32_t count)
		{
			glUniform2uiv(location, count, values);
		}

		void NpShader::setUniform3uiv(const int32_t location, const uint32_t* values, uint32_t count)
		{
			glUniform3uiv(location, count, values);
		}

		void NpShader::setUniform4uiv(const int32_t location, const uint32_t* values, uint32_t count)
		{
			glUniform4uiv(location, count, values);
		}

		void NpShader::setUniformM2fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix2fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM3fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix3fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM4fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix4fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM2x3fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix2x3fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM3x2fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix3x2fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM2x4fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix2x4fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM4x2fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix4x2fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM3x4fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix3x4fv(location, count, GL_FALSE, values);
		}

		void NpShader::setUniformM4x3fv(const int32_t location, const float* values, uint32_t count)
		{
			glUniformMatrix4x3fv(location, count, GL_FALSE, values);
		}

		void NpShader::initializeGl()
		{
			_uniformLocations.clear();
			++_initializationCount;
			_glProgramId = loadShaderFile(_path);
			if (getValid()) {
				PXE_INFO("Initialized PxeShader " + _path.string() + " with GL Id: " + std::to_string(_glProgramId));
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
				PXE_INFO("Uninitialized PxeShader " + _path.string());
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

		void NpShader::onDelete()
		{
			NpEngine::getInstance().removeShader(_path);
			PxeGLAsset::onDelete();
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
				if (msgLength == 0) msgLength = 1;
				char* msgString = (char*)alloca(msgLength);
				glGetShaderInfoLog(id, msgLength, NULL, msgString);
				if (msgLength == 1) msgString[0] = 0;
				PXE_INFO(std::string("Compiled ") + getShaderName(shaderType) + " shader: " + msgString);
			}
#endif
			return id;
		}

		uint32_t NpShader::loadShaderFile(const std::filesystem::path& path)
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
						PXE_ERROR("Failed to load PxeShader " + path.string() + " found unknown shader type \"" + line + "\"");
						return 0;
					}
				}
				else {
					ss[(int32_t)type] << line << "\n";
				}
			}

			stream.close();
			if (type == PxeShaderType::NONE) {
				PXE_ERROR("Failed to load PxeShader " + path.string() + " file empty or not found");
				return 0;
			}

			uint32_t programId = glCreateProgram();
			if (programId == 0) {
				PXE_ERROR("Failed to load PxeShader " + path.string() + +" failed to create Gl Program");
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
				PXE_ERROR("Failed to load PxeShader " + path.string() + +" failed to compile all shaders");
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
				PXE_ERROR("Failed to link shader " + path.string() + " " + errorString);
				glDeleteProgram(programId);
				return 0;
			}
#ifdef PXE_DEBUG
			else {
				int32_t msgLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &msgLength);
				if (msgLength == 0) msgLength = 1;
				char* msgString = (char*)alloca(msgLength);
				glGetProgramInfoLog(programId, msgLength, NULL, msgString);
				if (msgLength == 1) msgString[0] = 0;
				PXE_INFO("Linked shader " + path.string() + " " + msgString);
			}
#endif

			glValidateProgram(programId);
			glGetProgramiv(programId, GL_VALIDATE_STATUS, &res);
			if (res == GL_FALSE) {
				int32_t errorLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &errorLength);
				char* errorString = (char*)alloca(errorLength);
				glGetProgramInfoLog(programId, errorLength, NULL, errorString);
				PXE_ERROR("Failed to validate shader " + path.string() + " " + errorString);
				glDeleteProgram(programId);
				return 0;
			}
#ifdef PXE_DEBUG
			else {
				int32_t msgLength;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &msgLength);
				if (msgLength == 0) msgLength = 1;
				char* msgString = (char*)alloca(msgLength);
				glGetProgramInfoLog(programId, msgLength, NULL, msgString);
				if (msgLength == 1) msgString[0] = 0;
				PXE_INFO("Validated shader " + path.string() + " " + msgString);
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
					PXE_WARN("Failed to find uniform \"" + name + "\" in PxeShader " + _path.string());
				}

				return u;
			}
		}

		uint32_t NpShader::getGlProgramId() const
		{
			return _glProgramId;
		}

		const std::filesystem::path& NpShader::getShaderPath() const
		{
			return _path;
		}

		uint32_t NpShader::getAssetInitializationCount() const
		{
			return _initializationCount;
		}
	}
}