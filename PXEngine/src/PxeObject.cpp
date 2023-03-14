#include "PxeObject.h"

#include <new>
#include <shared_mutex>

#include "PxeLogger.h"
#include "PxeScene.h"

#define LOCK_SCOPE_READ() std::shared_lock objectScopeLock(impl()._objectMutex)
#define LOCK_SCOPE_WRITE() std::unique_lock objectScopeLock(impl()._objectMutex)

#define LOCK_READ() impl()._objectMutex.lock_shared()
#define UNLOCK_READ() impl()._objectMutex.unlock_shared()

#define LOCK_WRITE() impl()._objectMutex.lock()
#define UNLOCK_WRITE() impl()._objectMutex.unlock()

namespace pxengine {
	struct PxeObject::Impl {
		PxeScene* _parentScene;
		PxeComponent* _nextComponent;
		PxeSize _componentCount;
		glm::mat4 _transform;
		mutable std::shared_mutex _objectMutex;
	};

	PxeObject::PxeObject() {
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeObject::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeObject::STORAGE_ALIGN need be changed");

		new(&_storage) Impl();
		Impl& imp = impl();
		imp._parentScene = nullptr;
		imp._nextComponent = nullptr;
		imp._transform = glm::mat4(1.0f);
		imp._componentCount = 0;
	}

	PxeObject::~PxeObject() {
		{
			LOCK_SCOPE_WRITE();
			Impl& imp = impl();
			if (imp._parentScene) {
				PXE_ERROR("PxeObject destroyed without being removed from PxeScene");
			}

			PxeComponent* itr = imp._nextComponent;
			while (itr)
			{
				PxeComponent* component = itr;
				itr = component->_nextComponent;
				component->removeFromObject(*this);
				component->drop();
			}
		}

		impl().~Impl();
	}

	bool PxeObject::setScene(PxeScene& scene) {
		LOCK_SCOPE_WRITE();
		Impl& imp = impl();
		if (imp._parentScene != nullptr) {
			PXE_WARN("Attempted to add PxeObject to PxeScene that was already added to a PxeScene");
			return false;
		}

		imp._parentScene = &scene;
	}

	void PxeObject::clearScene(PxeScene& scene) {
		Impl& imp = impl();
		LOCK_SCOPE_WRITE();
		if (imp._parentScene != &scene) {
			PXE_ERROR("Attempted to remove PxeObject from incorrect or invalid PxeScene");
			return;
		}

		imp._parentScene = nullptr;
	}

	void PxeObject::addComponentsToScene()
	{
		LOCK_SCOPE_READ();
		Impl& imp = impl();
		PxeComponent* next = imp._nextComponent;
		while (next)
		{
			next->addToScene(*imp._parentScene);
			next = next->_nextComponent;
		}
	}

	void PxeObject::removeComponentsFromScene()
	{
		LOCK_SCOPE_READ();
		Impl& imp = impl();
		PxeComponent* next = imp._nextComponent;
		while (next)
		{
			next->removeFromScene(*imp._parentScene);
			next = next->_nextComponent;
		}
	}

	PXE_NODISCARD PxeObject* PxeObject::create()
	{
		return new(std::nothrow) PxeObject();
	}

	PXE_NODISCARD bool PxeObject::addComponent(PxeComponent& component)
	{
		Impl& imp = impl();

		{
			LOCK_SCOPE_WRITE();
			component.grab();
			if (!component.checkComponentRequirements(*this)) {
				PXE_WARN("Failed to add PxeComponent to PxeObject, component requirements failed");
				component.drop();
				return false;
			}

			component.addToObject(*this);
			++imp._componentCount;
			component._nextComponent = imp._nextComponent;
			imp._nextComponent = &component;
		}

		if (imp._parentScene)
			component.addToScene(*imp._parentScene);
		return true;
	}

	void PxeObject::removeComponent(PxeComponent& component)
	{
		LOCK_SCOPE_WRITE();
		if (component._parentObject != this) {
			PXE_WARN("Attempted to remove PxeComponent from incorrect/invalid PxeObject");
			return;
		}

		Impl& imp = impl();
		PxeComponent*& lastItr = imp._nextComponent;
		PxeComponent* itr = imp._nextComponent;
		while (itr)
		{
			if (itr == &component) {
				lastItr = component._nextComponent;
				goto componentRemoved;
			}

			lastItr = itr->_nextComponent;
			itr = itr->_nextComponent;
		}

		PXE_ERROR("Failed to remove PxeComponent from PxeObject, failed to find PxeComponent");
		return;

	componentRemoved:
		if (imp._parentScene)
			component.removeFromScene(*imp._parentScene);
		--imp._componentCount;
		component.removeFromObject(*this);
		component.drop();
	}

	PXE_NODISCARD PxeScene* PxeObject::getScene() const
	{
		LOCK_SCOPE_READ();
		return impl()._parentScene;
	}

	PXE_NODISCARD const glm::mat4& PxeObject::getTransform() const
	{
		LOCK_SCOPE_READ();
		return impl()._transform;
	}

	void PxeObject::setTransform(const glm::mat4& t)
	{
		LOCK_SCOPE_WRITE();
		impl()._transform = t;
	}

	PXE_NODISCARD bool PxeObject::hasComponent(PxeComponentId component) const
	{
		return getComponent(component);
	}

	PXE_NODISCARD bool PxeObject::hasExactComponent(PxeComponentId component) const
	{
		return getExactComponent(component);
	}

	PXE_NODISCARD PxeComponent* PxeObject::getComponent(PxeComponentId component, PxeSize index) const
	{
		LOCK_SCOPE_READ();
		PxeComponent* next = impl()._nextComponent;
		PxeSize curIndex = 0;
		while (next)
		{
			if (next->componentOfType(component) && curIndex++ == index)
				return next;
			next = next->_nextComponent;
		}

		return nullptr;
	}

	PXE_NODISCARD PxeComponent* PxeObject::getExactComponent(PxeComponentId component, PxeSize index) const
	{
		LOCK_SCOPE_READ();
		PxeComponent* next = impl()._nextComponent;
		PxeSize curIndex = 0;
		while (next)
		{
			if (next->getComponentId() == component && curIndex++ == index)
				return next;
			next = next->_nextComponent;
		}

		return nullptr;
	}

	PXE_NODISCARD PxeSize PxeObject::getComponentCount(PxeComponentId component) const
	{
		LOCK_SCOPE_READ();
		if (component == 0) return impl()._componentCount;
		PxeComponent* next = impl()._nextComponent;
		PxeSize count = 0;
		while (next)
		{
			if (next->componentOfType(component))
				++count;
			next = next->_nextComponent;
		}

		return count;
	}

	PXE_NODISCARD PxeSize PxeObject::getExactComponentCount(PxeComponentId component) const
	{
		LOCK_SCOPE_READ();
		PxeComponent* next = impl()._nextComponent;
		PxeSize count = 0;
		while (next)
		{
			if (next->getComponentId() == component)
				++count;
			next = next->_nextComponent;
		}

		return count;
	}

	PXE_NODISCARD PxeSize PxeObject::getComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset)
	{
		LOCK_SCOPE_READ();
		PxeComponent* next = impl()._nextComponent;
		PxeSize index = 0;
		while (next)
		{
			if (next->componentOfType(component)) {
				if (index >= offset) {
					componentBuffer[index - offset] = next;
					if (index - offset + 1 == componentBufferSize) break;
				}

				++index;
			}

			next = next->_nextComponent;
		}

		if (index <= offset) return 0;
		return index - offset;
	}

	PXE_NODISCARD PxeSize PxeObject::getExactComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset)
	{
		LOCK_SCOPE_READ();
		PxeComponent* next = impl()._nextComponent;
		PxeSize index = 0;
		while (next)
		{
			if (next->getComponentId() == component) {
				if (index >= offset) {
					componentBuffer[index - offset] = next;
					if (index - offset + 1 == componentBufferSize) break;
				}

				++index;
			}

			next = next->_nextComponent;
		}

		if (index <= offset) return 0;
		return index - offset;
	}
}