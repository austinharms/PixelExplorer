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
		// Note: this method calls lockWrite(), holding a write or read lock while calling this method will cause errors
		PXE_NODISCARD bool addComponent(PxeComponent& component);
		// Note: this method calls lockWrite(), holding a write or read lock while calling this method will cause errors
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
		PXE_NODISCARD PxeSize getComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0) const;
		PXE_NODISCARD PxeSize getExactComponents(PxeComponentId component, PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0) const;
		// Locks the object and prevents changes until unlock() is called
		// Note: this will not lock the object's transform
		// Note: you can call this recursively
		// Note: you must call unlock() for each call to lock()
		void lock() const;

		// Reverses the effects of a call to lock()
		void unlock() const;

		// Returns a pointer to a buffer of component pointers or nullptr on failure
		// Note: the buffer will be null terminated
		// Note: the returned buffer must be freed using free
		PXE_NODISCARD PxeComponent** getComponents(PxeComponentId component) const;

		// Returns a pointer to a buffer of component pointers or nullptr on failure
		// Note: the buffer will be null terminated
		// Note: the returned buffer must be freed using free
		PXE_NODISCARD PxeComponent** getExactComponents(PxeComponentId component) const;

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
		PXE_NODISCARD inline PxeSize getComponents(PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0) const
		{
			return getComponents(PxeComponent::getComponentId<T>(), componentBuffer, componentBufferSize, offset);
		}

		template<class T>
		PXE_NODISCARD inline PxeSize getExactComponents(PxeComponent** componentBuffer, PxeSize componentBufferSize, PxeSize offset = 0) const
		{
			return getExactComponents(PxeComponent::getComponentId<T>(), componentBuffer, componentBufferSize, offset);
		}

		// Returns a pointer to a buffer of component pointers or nullptr on failure
		// Note: the buffer will be null terminated
		// Note: the returned buffer must be freed using free
		template<class T>
		PXE_NODISCARD inline PxeComponent** getComponents() const
		{
			return getComponents(PxeComponent::getComponentId<T>());
		}

		// Returns a pointer to a buffer of component pointers or nullptr on failure
		// Note: the buffer will be null terminated
		// Note: the returned buffer must be freed using free
		template<class T>
		PXE_NODISCARD inline PxeComponent** getExactComponents() const
		{
			return getExactComponents(PxeComponent::getComponentId<T>());
		}

		virtual ~PxeObject();
		PXE_NOCOPY(PxeObject);

	private:
		PxeObject();
		static constexpr PxeSize STORAGE_SIZE = 160;
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