#include "PxeRenderComponent.h"

namespace pxengine {
	PxeRenderComponent::PxeRenderComponent(PxeRenderProperties& renderProperties): _renderProperties(renderProperties)
	{
		_renderProperties.grab();
	}

	PXE_NODISCARD const PxeRenderProperties& PxeRenderComponent::getRenderProperties() const
	{
		return _renderProperties;
	}

	PxeRenderComponent::~PxeRenderComponent()
	{
		_renderProperties.drop();
	}

	PXE_NODISCARD PxeRenderProperties& PxeRenderComponent::getRenderProperties()
	{
		return _renderProperties;
	}
}