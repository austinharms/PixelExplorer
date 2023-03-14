#include "PxesBasicShader.h"

#include <cassert>

#include "PxeFSHelpers.h"
#include "PxeRenderTarget.h"
#include "GL/glew.h"

PxesBasicShader::PxesBasicShader()
{
	_shaderProgram = nullptr;
	_textureLocation = -1;
}

PxesBasicShader::~PxesBasicShader()
{
}

PXE_NODISCARD pxengine::PxeShaderExecutionOrder PxesBasicShader::getExecutionOrder() const
{
	return pxengine::PxeDefaultShaderExecutionOrder::OPAQUE;
}

PXE_NODISCARD pxengine::PxeShaderId PxesBasicShader::getShaderId()
{
	return pxengine::PxeShader::getShaderId<PxesBasicShader>();
}

void PxesBasicShader::setTexture(uint32_t textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void PxesBasicShader::initializeGl()
{
	assert(pxengine::PxeGlShaderProgram::loadShaderProgram(pxengine::getAssetPath("shaders") / "basic.pxeshader", _shaderProgram));
	_textureLocation = _shaderProgram->getUniformLocation("u_Texture");
	_mvpLocation = _shaderProgram->getUniformLocation("u_MVP");
}

void PxesBasicShader::uninitializeGl()
{
	_textureLocation = -1;
	pxengine::PxeGlShaderProgram::unloadShaderProgram(_shaderProgram);
}

void PxesBasicShader::bind()
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
	glActiveTexture(GL_TEXTURE0);
	_shaderProgram->setUniform1i(_textureLocation, 0);
}

void PxesBasicShader::unbind()
{
	_shaderProgram->unbind();
}

void PxesBasicShader::setRenderTarget(pxengine::PxeRenderTarget& renderTarget)
{
	glViewport(0, 0, renderTarget.getRenderWidth(), renderTarget.getRenderHeight());
	pvMatrix = renderTarget.getRenderCamera()->getPVMatrix();
}

void PxesBasicShader::setObjectTransform(const glm::mat4& transform)
{
	_shaderProgram->setUniformM4f(_mvpLocation, pvMatrix * transform);
}
