#ifndef PXENGINE_UPDATE_COMPONENT_H_
#define PXENGINE_UPDATE_COMPONENT_H_
#include "PxeComponent.h"

namespace pxengine {
	class PxeScene;

	class PxeUpdateComponent : public PxeComponent
	{
	public:
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxeUpdateComponent, PxeComponent);
		virtual ~PxeUpdateComponent();
		PXE_NOCOPY(PxeUpdateComponent);

	protected:
		virtual void onUpdate() = 0;
		PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(PxeComponent);
		PxeUpdateComponent();
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeScene;
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_UPDATE_COMPONENT_H_
