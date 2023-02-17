#include "NpGuiRenderMaterial.h"

#include "NpEngine.h"
#include "PxeFSHelpers.h"
#include "NpLogger.h"
#include "GL/glew.h"

namespace pxengine {
	namespace nonpublic {
		NpGuiRenderMaterial::NpGuiRenderMaterial(PxeShader& shader) : PxeRenderMaterialInterface(PxeRenderPass::GUI), _guiShader(shader) {
			_projLocation = -1;
			_textureLocation = -1;
		}

		NpGuiRenderMaterial::~NpGuiRenderMaterial() {
			_guiShader.drop();
		}

		void NpGuiRenderMaterial::applyMaterial()
		{
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
			if (_textureLocation == -1)
				_textureLocation = glGetUniformLocation(_guiShader.getGlProgramId(), "u_Texture");
			glUniform1i(_textureLocation, 0);
		}

		PXE_NODISCARD PxeShader& NpGuiRenderMaterial::getShader() const
		{
			return _guiShader;
		}

		void NpGuiRenderMaterial::setProjectionMatrix(const glm::mat4& mat)
		{
			if (_projLocation == -1)
				_projLocation = glGetUniformLocation(_guiShader.getGlProgramId(), "u_Projection");
			glUniformMatrix4fv(_projLocation, 1, GL_FALSE, glm::value_ptr(mat));
		}

		void NpGuiRenderMaterial::setTexture(uint32_t texture)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
		}
	}
}