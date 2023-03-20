#include "PxeGlShaderProgram.h"

#include <fstream>
#include <string>
#include <new>

#include "GL/glew.h"
#include "PxeLogger.h"

namespace pxengine {
	PXE_NODISCARD uint32_t PxeGlShaderProgram::compileShader(uint32_t shaderType, const char* source)
	{
		uint32_t shaderId = glCreateShader(shaderType);
		glShaderSource(shaderId, 1, &source, nullptr);
		glCompileShader(shaderId);

		int32_t compileStatus; glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == GL_FALSE) {
			int32_t logLength; glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
			// add one to not allocate 0 bytes in case logLength is 0
			char* compileLog = static_cast<char*>(alloca(logLength + 1));
			compileLog[logLength] = '\0';
			glGetShaderInfoLog(shaderId, logLength, NULL, compileLog);
			PXE_ERROR("Failed to compile glShader: " + std::string(compileLog));
			glDeleteShader(shaderId);
			return 0;
		}
#ifdef PXE_DEBUG
		else {
			int32_t logLength; glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
			// add one to not allocate 0 bytes in case logLength is 0
			char* compileLog = static_cast<char*>(alloca(logLength + 1));
			compileLog[logLength] = '\0';
			glGetShaderInfoLog(shaderId, logLength, NULL, compileLog);
			PXE_INFO("Compiled glShader: " + std::string(compileLog));
		}
#endif

		return shaderId;
	}

	PXE_NODISCARD bool PxeGlShaderProgram::loadShaderProgram(const std::filesystem::path& shaderPath, PxeGlShaderProgram*& programOut)
	{
		std::ifstream fileStream(shaderPath);
		std::stringstream shaderSource;
		uint32_t currentShaderType = GL_NONE;
		std::string line;
		uint32_t glProgram = glCreateProgram();
		if (!glProgram) {
			PXE_ERROR("Failed to create glProgram");
			return false;
		}

		while (getline(fileStream, line)) {
			if (line.find("#shader") != std::string::npos) {
				uint32_t oldShaderType = currentShaderType;
				if (line.find("vertex") != std::string::npos) {
					currentShaderType = GL_VERTEX_SHADER;
				}
				else if (line.find("fragment") != std::string::npos) {
					currentShaderType = GL_FRAGMENT_SHADER;
				}
				else if (line.find("geometry") != std::string::npos) {

					currentShaderType = GL_GEOMETRY_SHADER;
				}
				else {
					PXE_ERROR("Failed to load PxeGlShaderProgram, found invalid shader type \"" + line + "\" in file " + shaderPath.string());
					glDeleteProgram(glProgram);
					return false;
				}

				const std::string sourceString = shaderSource.str();
				shaderSource.str("");
				if (sourceString.empty() || oldShaderType == GL_NONE) continue;
				uint32_t glShader = compileShader(oldShaderType, sourceString.c_str());
				if (!glShader) {
					PXE_ERROR("Failed to load PxeGlShaderProgram, one or more shaders in file " + shaderPath.string() + " failed to compile");
					glDeleteProgram(glProgram);
					return false;
				}

				glAttachShader(glProgram, glShader);
				// this wont delete the shader until the attached program is deleted 
				glDeleteShader(glShader);
			}
			else {
				shaderSource << line << "\n";
			}
		}

		if (currentShaderType == GL_NONE) {
			PXE_ERROR("Failed to load PxeGlShaderProgram from " + shaderPath.string() + ", file empty or not found");
			glDeleteProgram(glProgram);
			return false;
		}
		else {
			const std::string sourceString = shaderSource.str();
			shaderSource.str("");
			if (!sourceString.empty()) {
				uint32_t glShader = compileShader(currentShaderType, sourceString.c_str());
				if (!glShader) {
					PXE_ERROR("Failed to load PxeGlShaderProgram, one or more shaders in file " + shaderPath.string() + " failed to compile");
					glDeleteProgram(glProgram);
					return false;
				}

				glAttachShader(glProgram, glShader);
				// this wont delete the shader until the attached program is deleted 
				glDeleteShader(glShader);
			}
		}

		uint32_t oldProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, (int32_t*)&oldProgram);
		glLinkProgram(glProgram);
		glUseProgram(oldProgram);

		int32_t linkStatus;
		glGetProgramiv(glProgram, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE) {
			int32_t logLength; glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &logLength);
			// add one to not allocate 0 bytes in case logLength is 0
			char* linkLog = static_cast<char*>(alloca(logLength + 1));
			linkLog[logLength] = '\0';
			glGetProgramInfoLog(glProgram, logLength, nullptr, linkLog);
			PXE_ERROR("Failed to link shader file " + shaderPath.string() + " glProgram Log: " + linkLog);
			glDeleteProgram(glProgram);
			return false;
		}
#ifdef PXE_DEBUG
		else {
			int32_t logLength; glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &logLength);
			// add one to not allocate 0 bytes in case logLength is 0
			char* linkLog = static_cast<char*>(alloca(logLength + 1));
			linkLog[logLength] = '\0';
			glGetProgramInfoLog(glProgram, logLength, nullptr, linkLog);
			PXE_INFO("Linked shader file " + shaderPath.string() + " glProgramId: " + std::to_string(glProgram) + " glProgram Log: " + std::string(linkLog));
		}
#endif // PXE_DEBUG

		PxeGlShaderProgram* pxeShaderProgram = new(std::nothrow) PxeGlShaderProgram(glProgram);
		if (!pxeShaderProgram) {
			PXE_ERROR("Failed to allocate PxeGlShaderProgram");
			glDeleteProgram(glProgram);
			return false;
		}

		programOut = pxeShaderProgram;
		return true;
	}

	void PxeGlShaderProgram::unloadShaderProgram(PxeGlShaderProgram*& program)
	{
		if (!program) return;
		glDeleteProgram(program->getGlProgramId());
#ifdef PXE_DEBUG
		PXE_INFO("Deleted glProgram with id: " + std::to_string(program->getGlProgramId()));
#endif // PXE_DEBUG
		delete program;
		program = nullptr;
	}

	PxeGlShaderProgram::PxeGlShaderProgram(uint32_t glProgram) : _glProgramId(glProgram) {}

	void PxeGlShaderProgram::bind()
	{
		glUseProgram(_glProgramId);
	}

	void PxeGlShaderProgram::unbind()
	{
		glUseProgram(0);
	}

	PXE_NODISCARD int32_t PxeGlShaderProgram::getUniformLocation(const char* name) const
	{
		return glGetUniformLocation(_glProgramId, name);
	}

	PXE_NODISCARD uint32_t PxeGlShaderProgram::getGlProgramId() const
	{
		return _glProgramId;
	}

	void PxeGlShaderProgram::setUniform1fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniform1fv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform2fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniform2fv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform3fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniform3fv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform4fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniform4fv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform1iv(const int32_t location, const int32_t* values, uint32_t count) const
	{
		glUniform1iv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform2iv(const int32_t location, const int32_t* values, uint32_t count) const
	{
		glUniform2iv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform3iv(const int32_t location, const int32_t* values, uint32_t count) const
	{
		glUniform3iv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform4iv(const int32_t location, const int32_t* values, uint32_t count) const
	{
		glUniform4iv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform1uiv(const int32_t location, const uint32_t* values, uint32_t count) const
	{
		glUniform1uiv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform2uiv(const int32_t location, const uint32_t* values, uint32_t count) const
	{
		glUniform2uiv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform3uiv(const int32_t location, const uint32_t* values, uint32_t count) const
	{
		glUniform3uiv(location, count, values);
	}

	void PxeGlShaderProgram::setUniform4uiv(const int32_t location, const uint32_t* values, uint32_t count) const
	{
		glUniform4uiv(location, count, values);
	}

	void PxeGlShaderProgram::setUniformM2fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix2fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM3fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix3fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM4fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix4fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM2x3fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix2x3fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM3x2fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix3x2fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM2x4fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix2x4fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM4x2fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix4x2fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM3x4fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix3x4fv(location, count, GL_FALSE, values);
	}

	void PxeGlShaderProgram::setUniformM4x3fv(const int32_t location, const float* values, uint32_t count) const
	{
		glUniformMatrix4x3fv(location, count, GL_FALSE, values);
	}
}