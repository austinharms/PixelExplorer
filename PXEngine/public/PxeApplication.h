#ifndef PXENGINE_APPLICATION_H_
#define PXENGINE_APPLICATION_H_
#include "PxeTypes.h"
#include "PxeLogger.h"
#include "PxeEngine.h"

namespace pxengine {
	// TODO Comment this file
	class PxeApplication
	{
	public:
		void start(PxeLogInterface& logInterface);
		~PxeApplication();
		PXE_NOCOPY(PxeApplication);

	protected:
		virtual void onStart() = 0;
		virtual void onStop() = 0;
		virtual void onUpdate() = 0;
		virtual void postUpdate() {}
		virtual void prePhysics() {}
		virtual void postPhysics() {}
		PxeApplication();

	private:
		static constexpr PxeSize STORAGE_SIZE = 320;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		struct Impl;

		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		void applicationUpdateLoop();
		void physicsUpdateThreadLoop();
		void objectUpdateThreadLoop();
		void updateApplication();
		void updateRendering();
		void updatePhysics();
		void updateObjects();
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_APPLICATION_INTERFACE_H_