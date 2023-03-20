#include "PxeObject.h"

#include <new>
#include <mutex>

#include "PxeLogger.h"
#include "PxeScene.h"

namespace pxengine {
	struct PxeObject::Impl {
		PxeScene* _parentScene;
		PxeComponent* _nextComponent;
		glm::mat4 _transform;
		mutable std::recursive_mutex _objectMutex;
	};

	PxeObject::PxeObject() {
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeObject::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeObject::STORAGE_ALIGN need be changed");

		new(&_storage) Impl();
		Impl& imp = impl();
		imp._parentScene = nullptr;
		imp._nextComponent = nullptr;
		imp._transform = glm::mat4(1.0f);
	}

	PxeObject::~PxeObject() {
		{
			Impl& imp = impl();
			std::lock_guard lock(imp._objectMutex);
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
		Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		if (imp._parentScene != nullptr) {
			PXE_WARN("Attempted to add PxeObject to PxeScene that was already added to a PxeScene");
			return false;
		}

		imp._parentScene = &scene;
		return true;
	}

	void PxeObject::clearScene(PxeScene& scene) {
		Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		if (imp._parentScene != &scene) {
			PXE_ERROR("Attempted to remove PxeObject from incorrect or invalid PxeScene, PxeObject not removed");
			return;
		}

		imp._parentScene = nullptr;
	}

	void PxeObject::addComponentsToScene()
	{
		Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
		while (next)
		{
			next->addToScene(*imp._parentScene);
			next = next->_nextComponent;
		}
	}

	void PxeObject::removeComponentsFromScene()
	{
		Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
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
		std::lock_guard lock(imp._objectMutex);
		component.grab();
		if (!component.checkComponentRequirements(*this)) {
			PXE_WARN("Failed to add PxeComponent to PxeObject, object did not meet component requirements");
			component.drop();
			return false;
		}

		component.addToObject(*this);
		component._nextComponent = imp._nextComponent;
		imp._nextComponent = &component;
		if (imp._parentScene)
			component.addToScene(*imp._parentScene);
		return true;
	}

	void PxeObject::removeComponent(PxeComponent& component)
	{
		if (component._parentObject != this) {
			PXE_WARN("Attempted to remove PxeComponent from incorrect PxeObject");
			return;
		}

		Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent*& lastItr = imp._nextComponent;
		PxeComponent* itr = imp._nextComponent;
		while (itr)
		{
			if (itr == &component) {
				if (imp._parentScene)
					component.removeFromScene(*imp._parentScene);
				// Remove component from list
				lastItr = component._nextComponent;
				component.removeFromObject(*this);
				component.drop();
				return;
			}

			lastItr = itr->_nextComponent;
			itr = itr->_nextComponent;
		}

		PXE_ERROR("Failed to remove PxeComponent from PxeObject, failed to find PxeComponent in PxeObject's component list");
	}

	PXE_NODISCARD PxeScene* PxeObject::getScene() const
	{
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		return impl()._parentScene;
	}

	PXE_NODISCARD const glm::mat4& PxeObject::getTransform() const
	{
		return impl()._transform;
	}

	void PxeObject::setTransform(const glm::mat4& t)
	{
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
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
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
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
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
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
		PxeSize count = 0;
		while (next)
		{
			if (component == 0 || next->componentOfType(component))
				++count;
			next = next->_nextComponent;
		}

		return count;
	}

	PXE_NODISCARD PxeSize PxeObject::getExactComponentCount(PxeComponentId component) const
	{
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
		PxeSize count = 0;
		while (next)
		{
			if (next->getComponentId() == component)
				++count;
			next = next->_nextComponent;
		}

		return count;
	}

	PXE_NODISCARD PxeSize PxeObject::getComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset) const
	{
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
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

	PXE_NODISCARD PxeSize PxeObject::getExactComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset) const
	{
		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeComponent* next = imp._nextComponent;
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

	void PxeObject::lock() const
	{
		impl()._objectMutex.lock();
	}

	void PxeObject::unlock() const
	{
		impl()._objectMutex.unlock();
	}

	// TODO is this faster then getting the count and preallocating the buffer 
	PXE_NODISCARD PxeComponent** PxeObject::getComponents(PxeComponentId component) const
	{
		struct StackNode
		{
			StackNode(StackNode* nextNode) {
				next = nextNode;
				component = nullptr;
			}

			StackNode* next;
			PxeComponent* component;
		};

		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		int64_t nodeCount = 0;
		PxeComponent* itr = imp._nextComponent;
		StackNode* nodeItr = nullptr;
	createNode:
		nodeItr = new(alloca(sizeof(StackNode))) StackNode(nodeItr);
		++nodeCount;

		while (itr)
		{
			if (itr->componentOfType(component)) {
				nodeItr->component = itr;
				itr = itr->_nextComponent;
				goto createNode;
			}

			itr = itr->_nextComponent;
		}

		PxeComponent** buf = static_cast<PxeComponent**>(malloc(sizeof(PxeComponent*) * nodeCount));
		if (!buf) {
			PXE_ERROR("Failed to allocate PxeComponent* buffer");
			return nullptr;
		}

		for (int64_t i = nodeCount - 1; i >= 0; i--)
		{
			buf[i] = nodeItr->component;
			nodeItr = nodeItr->next;
		}

		return buf;
	}

	// TODO is this faster then getting the count and preallocating the buffer 
	PXE_NODISCARD PxeComponent** PxeObject::getExactComponents(PxeComponentId component) const
	{
		struct StackNode
		{
			StackNode(StackNode* nextNode) {
				next = nextNode;
				component = nullptr;
			}

			StackNode* next;
			PxeComponent* component;
		};

		const Impl& imp = impl();
		std::lock_guard lock(imp._objectMutex);
		PxeSize nodeCount = 0;
		PxeComponent* itr = imp._nextComponent;
		StackNode* nodeItr = nullptr;
	createNode:
		nodeItr = new(alloca(sizeof(StackNode))) StackNode(nodeItr);
		++nodeCount;

		while (itr)
		{
			if (itr->getComponentId() == component) {
				nodeItr->component = itr;
				itr = itr->_nextComponent;
				goto createNode;
			}

			itr = itr->_nextComponent;
		}

		PxeComponent** buf = static_cast<PxeComponent**>(malloc(sizeof(PxeComponent*) * nodeCount));
		if (!buf) {
			PXE_ERROR("Failed to allocate PxeComponent* buffer");
			return nullptr;
		}

		for (PxeSize i = nodeCount - 1; i >= 0; i--)
		{
			buf[i] = nodeItr->component;
			nodeItr = nodeItr->next;
		}

		return buf;
	}
}