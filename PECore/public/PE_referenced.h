#ifndef PE_REFERENCED_H_
#define PE_REFERENCED_H_
#include "PE_defines.h"
#include "SDL_atomic.h"

namespace pe {
	// Base class for all resources that support sharing/referenced by multiple other objects
	// You must call grab when storing a reference to the object and then call drop when the object is no longer referenced
	// All instances are created with a reference count of 1 and when no longer referenced aka reference count is zero, OnDrop is called and the object should cleanup/delete itself
	class PE_API Referenced {
	public: 
		typedef decltype(SDL_AtomicInt::value) Count;
		virtual ~Referenced() = default;
		// Increases the reference count by 1
		void Grab();
		// Decreases the reference count by 1
		void Drop();
		// Returns the current reference count
		PE_NODISCARD Count GetReferenceCount() const;

	protected:
		Referenced();
		// Call automatically when reference count is zero
		// When called the object should cleanup/delete itself
		virtual void OnDrop() = 0;

	private:
		SDL_AtomicInt atomic_reference_count_;
	};
}
#endif // !PE_REFERENCED_H_
