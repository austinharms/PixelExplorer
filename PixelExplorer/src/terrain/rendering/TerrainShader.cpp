#include "TerrainShader.h"

#include "Log.h"
#include "PxeFSHelpers.h"
#include "PxeRenderTarget.h"
#include "GL/glew.h"

namespace pixelexplorer {
	namespace terrain {
		TerrainShader::TerrainShader()
		{
			_shaderProgram = nullptr;
			_uMVPLocation = -1;
			_uCameraLocation = -1;
			_uLightDirectionLocation = -1;
			_uLightColorLocation = -1;
		}

		TerrainShader::~TerrainShader()
		{
		}

		PXE_NODISCARD pxengine::PxeShaderExecutionOrder TerrainShader::getExecutionOrder() const
		{
			return pxengine::PxeDefaultShaderExecutionOrder::OPAQUE;
		}

		PXE_NODISCARD pxengine::PxeShaderId TerrainShader::getShaderId()
		{
			return pxengine::PxeShader::getShaderId<TerrainShader>();
		}

		void TerrainShader::setLightDirection(const glm::vec3& dir)
		{
			_shaderProgram->setUniform3f(_uLightDirectionLocation, dir);
		}

		void TerrainShader::setLightColor(const glm::vec3& color)
		{
			_shaderProgram->setUniform3f(_uLightColorLocation, color);
		}

		void TerrainShader::initializeGl()
		{
			if (!pxengine::PxeGlShaderProgram::loadShaderProgram(pxengine::getAssetPath("shaders") / "terrain.pxeshader", _shaderProgram)) {
				PEX_FATAL("Failed to load terrain PxeGlShaderProgram");
			}

			_uMVPLocation = _shaderProgram->getUniformLocation("u_MVP");
			_uCameraLocation = _shaderProgram->getUniformLocation("u_CameraPos");
			_uLightDirectionLocation = _shaderProgram->getUniformLocation("u_LightDir");
			_uLightColorLocation = _shaderProgram->getUniformLocation("u_LightColor");
		}

		void TerrainShader::uninitializeGl()
		{
			_uMVPLocation = -1;
			_uCameraLocation = -1;
			_uLightDirectionLocation = -1;
			_uLightColorLocation = -1;
			pxengine::PxeGlShaderProgram::unloadShaderProgram(_shaderProgram);
		}

		void TerrainShader::bind()
		{
			_shaderProgram->bind();
			glDisable(GL_SCISSOR_TEST);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_BLEND);
			glDisable(GL_PRIMITIVE_RESTART);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		void TerrainShader::unbind()
		{
			_shaderProgram->unbind();
		}

		void TerrainShader::setRenderTarget(pxengine::PxeRenderTarget& renderTarget)
		{
			glViewport(0, 0, renderTarget.getRenderWidth(), renderTarget.getRenderHeight());
			pvMatrix = renderTarget.getRenderCamera()->getPVMatrix();
			_shaderProgram->setUniform3f(_uCameraLocation, renderTarget.getRenderCamera()->getPosition());
		}

		void TerrainShader::setObjectTransform(const glm::mat4& transform)
		{
			_shaderProgram->setUniformM4f(_uMVPLocation, pvMatrix * transform);
		}
	}
}
