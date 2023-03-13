#include "PxeGuiShader.h"

#include "PxeLogger.h"
#include "PxeFSHelpers.h"
#include "GL/glew.h"
#include "imgui.h"
#include "PxeEngine.h"
#include "PxeRenderPipeline.h"
#include "PxeGlImGuiBackend.h"

namespace pxengine {
	PxeGuiShader::PxeGuiShader()
	{
		_glProgram = nullptr;
		_projectionLocation = -1;
		_textureLocation = -1;
	}

	PxeGuiShader::~PxeGuiShader()
	{
	}

	PXE_NODISCARD PxeShaderId PxeGuiShader::getShaderId()
	{
		return PxeShader::getShaderId<PxeGuiShader>();
	}

	PXE_NODISCARD PxeShaderExecutionOrder PxeGuiShader::getExecutionOrder() const
	{
		return PxeDefaultShaderExecutionOrder::GUI;
	}

	void PxeGuiShader::setProjectionMatrix(const glm::mat4& mat)
	{
		_glProgram->setUniformM4f(_projectionLocation, mat);
	}

	void PxeGuiShader::setTexture(uint32_t texture)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	void PxeGuiShader::initializeGl()
	{
		if (!PxeGlShaderProgram::loadShaderProgram(getAssetPath("shaders") / "pxengine_gui_shader.pxeshader", _glProgram)) {
			PXE_FATAL("Failed to load gui shader program");
		}

		if ((_projectionLocation = _glProgram->getUniformLocation("u_Projection")) == -1) {
			PXE_FATAL("Failed to load gui shader u_Projection uniform location");
		}

		if ((_textureLocation = _glProgram->getUniformLocation("u_Texture")) == -1) {
			PXE_FATAL("Failed to load gui shader u_Texture uniform location");
		}
	}

	void PxeGuiShader::uninitializeGl()
	{
		PxeGlShaderProgram::unloadShaderProgram(_glProgram);
		_projectionLocation = -1;
		_textureLocation = -1;
	}

	void PxeGuiShader::bind()
	{
		if (!ImGui::GetCurrentContext()) {
			PXE_FATAL("Attempted to use PxeGuiShader with invalid gui context bound");
		}

		_glProgram->bind();
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_SCISSOR_TEST);
		glDisable(GL_PRIMITIVE_RESTART);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindSampler(0, 0);
		glActiveTexture(GL_TEXTURE0);
		_glProgram->setUniform1i(_textureLocation, 0);
		ImGui::NewFrame();
	}

	void PxeGuiShader::unbind()
	{
		ImGui::Render();
		PxeEngine::getInstance().getRenderPipeline().getGuiBackend()->renderDrawData();
		_glProgram->unbind();
	}
}