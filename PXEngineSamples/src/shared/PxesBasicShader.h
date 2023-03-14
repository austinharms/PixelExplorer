#ifndef PXESAMPLES_BASIC_SHADER_H_
#define PXESAMPLES_BASIC_SHADER_H_
#include "PxeGlShaderProgram.h"
#include "PxeShader.h"

class PxesBasicShader : public pxengine::PxeShader
{
public:
	PxesBasicShader();
	virtual ~PxesBasicShader();
	PXE_NODISCARD pxengine::PxeShaderExecutionOrder getExecutionOrder() const override;
	PXE_NODISCARD pxengine::PxeShaderId getShaderId() override;
	void setTexture(uint32_t textureId);

protected:
	void initializeGl() override;
	void uninitializeGl() override;
	void bind() override;
	void unbind() override;
	void setRenderTarget(pxengine::PxeRenderTarget& renderTarget) override;
	void setObjectTransform(const glm::mat4& transform) override;

private:
	pxengine::PxeGlShaderProgram* _shaderProgram;
	int32_t _textureLocation;
	int32_t _mvpLocation;
	glm::mat4 pvMatrix;
};
#endif // !PXESAMPLES_BASIC_SHADER_H_
