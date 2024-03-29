#ifndef PXENGINE_REFCOUNT_H_
#define PXENGINE_REFCOUNT_H_
#include <atomic>

#include "PxeTypes.h"

namespace pxengine {
	// Reference counting helper class that counts how many times the object has been "grabbed" or "dropped" and calls delete when the object is no longer reference
	// Note: When instantiated the reference count starts at 1 and you must call drop when it's no logger needed
	// Note: Most PXEngine Objects inherit from this class and you must call grab if you store a reference to them and drop when that reference is no logger needed/stored
	// Note: All objects that inherit from this class MUST be allocated using new as delete is called when the object is dropped
	class PxeRefCount
	{
	public:
		// Returns the number of PxeRefCount instances that currently exist
		// Note: this only works if PXE_DEBUG is defined
		static PxeSize getPxeRefCountInstanceCount();

		// Increment the reference count by 1
		// Note: This insures the object will not be deleted until drop is called
		void grab();

		// Decrement the reference count by 1
		// Returns true if the reference count is 0 and the object was deleted
		bool drop();

		// Returns the current reference count
		PXE_NODISCARD PxeSize getRefCount();

		PxeRefCount();
		virtual ~PxeRefCount();
		PxeRefCount(const PxeRefCount& other) = delete;
		PxeRefCount operator=(const PxeRefCount& other) = delete;

	protected:
		// This is called before the object is deleted
		// Note: if grab is called in this function it behaves like it was called before the last drop
		// Warning: calling drop in this function may cause recursive behavior 
		virtual void onDelete() {}
		
	private:
		std::atomic<PxeSize> _refCount;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
#ifdef PXE_DEBUG
		static std::atomic<PxeSize> s_totalCount;
#endif // PXE_DEBUG
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_REFCOUNT_H_
