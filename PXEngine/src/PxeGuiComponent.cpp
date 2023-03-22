#include "PxeGuiComponent.h"

#include "PxeEngine.h"
#include "PxeRenderPipeline.h"

namespace pxengine {
	PxeGuiComponent::PxeGuiComponent() : PxeRenderComponent(PxeEngine::getInstance().getRenderPipeline().getGuiRenderProperties())
	{
	}
	
	PxeGuiComponent::~PxeGuiComponent()
	{
	}
}