#ifndef PXENGINE_PHYSICS_SHAPE_H_
#define PXENGINE_PHYSICS_SHAPE_H_
#include "PxeComponent.h"
#include "PxShape.h"

namespace pxengine {
	// TODO Comment file
	class PxePhysicsShape : public PxeComponent
	{
	public:
		PXE_NODISCARD physx::PxShape& getShape();

		PxePhysicsShape(physx::PxShape& shape);
		virtual ~PxePhysicsShape();
		PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(PxePhysicsShape, PxeComponent);

	protected:
		PXE_NODISCARD virtual bool checkComponentRequirements(pxengine::PxeObject& object) override;
		virtual void addToObject(pxengine::PxeObject& object) override;
		virtual void removeFromObject(pxengine::PxeObject& object) override;
		virtual void addToScene(pxengine::PxeScene& scene) override;
		virtual void removeFromScene(pxengine::PxeScene& scene) override;

	private:
		physx::PxShape& _shape;
	};
}
#endif // !PXENGINE_PHYSICS_SHAPE_H_
