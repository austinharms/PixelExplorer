#include "PxeComponent.h"

#include "PxeLogger.h"
#include "PxeScene.h"

namespace pxengine {
	std::mutex PxeComponent::s_componentIdMutex;
	uint32_t PxeComponent::s_componentIdCounter = 0;

	PxeComponent::PxeComponent()
	{
		_parentObject = nullptr;
		_nextComponent = nullptr;
	}

	PXE_NODISCARD bool PxeComponent::checkComponentRequirements(PxeObject& object)
	{
		if (_parentObject) {
			PXE_WARN("Component requirements failed, PxeComponent already added to PxeObject");
		}

		return !_parentObject;
	}

	PxeComponent::~PxeComponent()
	{
		if (_parentObject != nullptr)
		{
			PXE_ERROR("PxeComponent destroyed without being removed from PxeObject");
		}
	}

	PXE_NODISCARD PxeComponentId PxeComponent::getComponentId() const {
		return PxeComponent::getComponentId<PxeComponent>();
	}

	PXE_NODISCARD bool PxeComponent::componentOfType(PxeComponentId id) const {
		return getComponentId<PxeComponent>() == id;
	}

	void PxeComponent::addToObject(PxeObject& object)
	{
		_parentObject = &object;
	}

	void PxeComponent::removeFromObject(PxeObject& object)
	{
		if (&object != _parentObject) {
			PXE_ERROR("Removed PxeComponent from incorrect PxeObject");
		}

		_parentObject = nullptr;
	}

	PXE_NODISCARD PxeObject* PxeComponent::getParentObject() const
	{
		return _parentObject;
	}

	void PxeComponent::addToScene(PxeScene& scene)
	{
		scene.addComponent(*this);
	}

	void PxeComponent::removeFromScene(PxeScene& scene)
	{
		scene.removeComponent(*this);
	}

	PxeComponentId PxeComponent::getNextComponentId()
	{
		std::lock_guard<std::mutex> lock(s_componentIdMutex);
		return ++s_componentIdCounter;
	}
}