#include "PxePhysicsShape.h"

#include "PxePhysicsActor.h"
#include "PxeObject.h"

namespace pxengine {
    PxePhysicsShape::PxePhysicsShape(physx::PxShape& shape) : _shape(shape)
    {
        _shape.acquireReference();
    }

    PxePhysicsShape::~PxePhysicsShape()
    {
        _shape.release();
    }

    PXE_NODISCARD physx::PxShape& PxePhysicsShape::getShape()
    {
        return _shape;
    }

    PXE_NODISCARD bool PxePhysicsShape::checkComponentRequirements(pxengine::PxeObject& object)
    {
        return PxeComponent::checkComponentRequirements(object);
    }

    void PxePhysicsShape::addToObject(pxengine::PxeObject& object)
    {
        PxePhysicsActor* actor = object.getComponent<PxePhysicsActor>();
        if (actor) {
            actor->addShape(*this);
        }
        else {
            //PXE_WARN("Failed to find PxePhysicsActor, PxePhysicsActor should be added before PxePhysicsShapes as PxePhysicsShapes do not work without a PxePhysicsActor");
        }

        PxeComponent::addToObject(object);
    }

    void PxePhysicsShape::removeFromObject(pxengine::PxeObject& object)
    {
        PxePhysicsActor* actor = object.getComponent<PxePhysicsActor>();
        if (actor) actor->removeShape(*this);
        PxeComponent::removeFromObject(object);
    }

    void PxePhysicsShape::addToScene(pxengine::PxeScene& scene)
    {
        PxeComponent::addToScene(scene);
    }

    void PxePhysicsShape::removeFromScene(pxengine::PxeScene& scene)
    {
        PxeComponent::removeFromScene(scene);
    }
}