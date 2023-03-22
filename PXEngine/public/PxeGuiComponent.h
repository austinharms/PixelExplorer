#ifndef PXENGINE_GUI_COMPONENT_H_
#define PXENGINE_GUI_COMPONENT_H_
#include "PxeRenderComponent.h"

namespace pxengine {
	class PxeGuiComponent : public PxeRenderComponent
	{
	public:
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxeGuiComponent, PxeRenderComponent);
		virtual ~PxeGuiComponent();
		PXE_NOCOPY(PxeGuiComponent);

	protected:
		virtual void onRender() override = 0;
		PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(PxeRenderComponent);
		PxeGuiComponent();
	};
}
#endif // !PXENGINE_GUI_COMPONENT_H_
