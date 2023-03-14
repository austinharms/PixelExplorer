#ifndef PXESAMPLES_BASIC_RENDER_PROPERTIES_H_
#define PXESAMPLES_BASIC_RENDER_PROPERTIES_H_
#include "PxeRenderProperties.h"
#include "PxeShader.h"
#include "PxesBasicShader.h"
#include "PxeTexture.h"

class PxesBasicRenderProperties : public pxengine::PxeRenderProperties
{
public:
	PxesBasicRenderProperties(pxengine::PxeTexture& texture);
	virtual ~PxesBasicRenderProperties();
	PXE_NODISCARD pxengine::PxeShader* getShader() const override;

protected:
	void onApplyProperties() override;

private:
	PxesBasicShader* _basicShader;
	pxengine::PxeTexture& _texture;
};
#endif // !PXESAMPLES_BASIC_RENDER_PROPERTIES_H_
