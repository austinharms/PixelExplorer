#ifndef PXENGINE_OBJECT_H_
#define PXENGINE_OBJECT_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeComponent.h"

namespace pxengine {
	class PxeScene;

	// TODO Comment this file
	class PxeObject : public PxeRefCount
	{
	public:
		// Returns a new PxeObject instance or nullptr on failure
		PXE_NODISCARD static PxeObject* create();
		PXE_NODISCARD bool addComponent(PxeComponent& component);
		void removeComponent(PxeComponent& component);
		PXE_NODISCARD PxeScene* getScene() const;
		PXE_NODISCARD const glm::mat4& getTransform() const;
		void setTransform(const glm::mat4& t);
		PXE_NODISCARD bool hasComponent(PxeComponentId component) const;
		PXE_NODISCARD bool hasExactComponent(PxeComponentId component) const;
		PXE_NODISCARD PxeComponent* getComponent(PxeComponentId component, PxeSize index = 0) const;
		PXE_NODISCARD PxeComponent* getExactComponent(PxeComponentId component, PxeSize index = 0) const;
		PXE_NODISCARD PxeSize getComponentCount(PxeComponentId component = 0) const;
		PXE_NODISCARD PxeSize getExactComponentCount(PxeComponentId component) const;
		PXE_NODISCARD PxeSize getComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0);
		PXE_NODISCARD PxeSize getExactComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0);

		template<class T>
		PXE_NODISCARD inline bool hasComponent() const {
			return hasComponent(PxeComponent::getComponentId<T>());
		}

		template<class T>
		PXE_NODISCARD inline bool hasExactComponent() const
		{
			return hasExactComponent(PxeComponent::getComponentId<T>());
		}

		template<class T>
		PXE_NODISCARD inline T* getComponent(PxeSize index = 0) const
		{
			return static_cast<T*>(getComponent(PxeComponent::getComponentId<T>(), index));
		}

		template<class T>
		PXE_NODISCARD inline T* getExactComponent(PxeSize index = 0) const
		{
			return static_cast<T*>(getExactComponent(PxeComponent::getComponentId<T>(), index));
		}

		template<class T>
		PXE_NODISCARD inline PxeSize getComponentCount() const
		{
			return getComponentCount(PxeComponent::getComponentId<T>());
		}

		template<class T>
		PXE_NODISCARD inline PxeSize getExactComponentCount() const
		{
			return getExactComponentCount(PxeComponent::getComponentId<T>());
		}

		template<class T>
		PXE_NODISCARD inline PxeSize getComponents(PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0)
		{
			return getComponents(PxeComponent::getComponentId<T>(), componentBuffer, componentBufferSize, offset);
		}

		template<class T>
		PXE_NODISCARD inline PxeSize getExactComponents(PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0)
		{
			return getExactComponents(PxeComponent::getComponentId<T>(), componentBuffer, componentBufferSize, offset);
		}

		virtual ~PxeObject();
		PXE_NOCOPY(PxeObject);

	private:
		PxeObject();
		static constexpr PxeSize STORAGE_SIZE = 96;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeScene;
		struct Impl;

		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		void addComponentsToScene();
		void removeComponentsFromScene();
		bool setScene(PxeScene& scene);
		void clearScene(PxeScene& scene);
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_OBJECT_H_