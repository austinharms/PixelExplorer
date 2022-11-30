#include <stdint.h>
#include <atomic>

#ifndef PXENGINE_REFCOUNT_H_
#define PXENGINE_REFCOUNT_H_
namespace pxengine {
	// a class used to track the amount of reference that exist to this object and to delete its self when no longer referenced
	class PxeRefCount
	{
	public:
		PxeRefCount();
		virtual ~PxeRefCount();

		// increment the reference count
		// this insures the object will not be deleted until drop is called
		void grab();

		// decrement the reference count
		// returns true if the object was deleted (reference count was 0)
		bool drop();

		// returns the current reference count (uint32_t)
		size_t getRefCount();

		PxeRefCount(const PxeRefCount& other) = delete;
		PxeRefCount operator=(const PxeRefCount& other) = delete;

	protected:
		// this is called before the object is deleted
		// if grab is called in this function the object will not be deleted and drop will return false as expected if grab was called before the last drop was called
		// warning calling drop in this function may cause recursive behavior 
		virtual void onDelete() {}

	private:
		std::atomic<size_t> _refCount;
	};
}
#endif // !PXENGINE_REFCOUNT_H_
