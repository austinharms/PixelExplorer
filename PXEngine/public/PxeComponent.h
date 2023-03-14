#ifndef PXENGINE_COMPONENT_H_
#define PXENGINE_COMPONENT_H_
PXE_PRIVATE_IMPLEMENTATION_START
#include <mutex>
PXE_PRIVATE_IMPLEMENTATION_END
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeObject;
	class PxeScene;

	// TODO Comment this file
	class PxeComponent : public PxeRefCount
	{
	public:
		PXE_NODISCARD virtual PxeComponentId getComponentId() const;

		PXE_NODISCARD virtual bool componentOfType(PxeComponentId id) const;

		template<class T, typename std::enable_if<std::is_base_of<PxeComponent, T>::value>::type* = nullptr>
		PXE_NODISCARD static PxeComponentId getComponentId() {
			static PxeComponentId componentId = 0;
			if (componentId == 0) componentId = getNextComponentId();
			return componentId;
		}

		template<>
		PXE_NODISCARD static PxeComponentId getComponentId<PxeComponent>() { return 0; }

		template<class T, typename std::enable_if<std::is_base_of<PxeComponent, T>::value>::type* = nullptr>
		PXE_NODISCARD inline bool componentOfType()
		{
			return componentOfType(getComponentId<T>());
		}

		PXE_NODISCARD PxeObject* getParentObject() const;

		virtual ~PxeComponent();
		PXE_NOCOPY(PxeComponent);

	protected:
		PxeComponent();
		PXE_NODISCARD virtual bool checkComponentRequirements(PxeObject& object);
		virtual void addToObject(PxeObject& object);
		virtual void removeFromObject(PxeObject& object);
		virtual void addToScene(PxeScene& scene);
		virtual void removeFromScene(PxeScene& scene);

	private:
		static PxeComponentId getNextComponentId();
		PxeObject* _parentObject;
		// PxeComponents double as linked list nodes
		// Use by PxeObject to store attached components
		PxeComponent* _nextComponent;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeObject;
		static std::mutex s_componentIdMutex;
		static PxeComponentId s_componentIdCounter;
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}

#define PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(className, parentClassName)	PXE_NODISCARD virtual pxengine::PxeComponentId getComponentId() const override { return pxengine::PxeComponent::getComponentId<className>(); } \
																				PXE_NODISCARD virtual bool componentOfType(pxengine::PxeComponentId id) const override { return className::getComponentId() == id || parentClassName::componentOfType(id); }

#define PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(parentClassName)	PXE_NODISCARD virtual bool checkComponentRequirements(pxengine::PxeObject& object) override { return parentClassName::checkComponentRequirements(object); } \
																		virtual void addToObject(pxengine::PxeObject& object) override { parentClassName::addToObject(object); } \
																		virtual void removeFromObject(pxengine::PxeObject& object) override { parentClassName::removeFromObject(object); } \
																		virtual void addToScene(pxengine::PxeScene& scene) override { parentClassName::addToScene(scene); } \
																		virtual void removeFromScene(pxengine::PxeScene& scene) override { parentClassName::removeFromScene(scene); }
#endif // !PXENGINE_COMPONENT_H_
