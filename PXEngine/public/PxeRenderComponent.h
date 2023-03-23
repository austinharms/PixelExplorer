#ifndef PXENGINE_RENDER_COMPONENT_H_
#define PXENGINE_RENDER_COMPONENT_H_
#include "PxeTypes.h"
#include "PxeComponent.h"
#include "PxeRenderProperties.h"

namespace pxengine {
	class PxeRenderPipeline;

	class PxeRenderComponent : public PxeComponent
	{
	public:
		PXE_NODISCARD const PxeRenderProperties& getRenderProperties() const;
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxeRenderComponent, PxeComponent);
		virtual ~PxeRenderComponent();
		PXE_NOCOPY(PxeRenderComponent);
		
	protected:
		virtual void onRender() = 0;
		PXE_NODISCARD PxeRenderProperties& getRenderProperties();
		PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(PxeComponent);
		PxeRenderComponent(PxeRenderProperties& renderProperties);

	private:
		PxeRenderProperties& _renderProperties;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeRenderPipeline;
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_RENDER_COMPONENT_H_
