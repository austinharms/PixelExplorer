#include "PxesRenderDataComponent.h"

PxesRenderDataComponent::PxesRenderDataComponent(PxesRenderData& renderData, pxengine::PxeRenderProperties& renderProperties) :
	pxengine::PxeRenderComponent(renderProperties), _renderData(renderData)
{
	renderData.grab();
}

PxesRenderDataComponent::~PxesRenderDataComponent()
{
	_renderData.drop();
}

void PxesRenderDataComponent::onRender()
{
	_renderData.draw();
}


