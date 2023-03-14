#ifndef PXESAMPLES_RENDER_DATA_COMPONENT_H_
#define PXESAMPLES_RENDER_DATA_COMPONENT_H_
#include "PxeRenderComponent.h"
#include "PxesRenderData.h"

class PxesRenderDataComponent : public pxengine::PxeRenderComponent
{
public:
	PxesRenderDataComponent(PxesRenderData& renderData, pxengine::PxeRenderProperties& renderProperties);
	virtual ~PxesRenderDataComponent();
	PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxesRenderDataComponent, pxengine::PxeRenderComponent);

protected:
	PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(pxengine::PxeRenderComponent);
	void onRender() override;

private:
	PxesRenderData& _renderData;
};
#endif // !PXESAMPLES_RENDER_DATA_COMPONENT_H_
