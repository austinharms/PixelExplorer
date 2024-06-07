// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_REFERENCED_H_
#define PE_REFERENCED_H_
#include "PE_defines.h"
#include "PE_stdint.h"

namespace pe {
	// Base class for all resources that support sharing/referenced by multiple other objects
	// You must call grab when storing a reference to the object and then call drop when the object is no longer referenced
	// All instances are created with a reference count of 1 and when no longer referenced aka reference count is zero, OnDrop is called and the object should cleanup/delete itself
	class PE_API Referenced {
	public:
		typedef uint32_t RefCount;
		virtual ~Referenced() = default;
		// Increases the reference count by 1
		void Grab();
		// Decreases the reference count by 1
		void Drop();
		// Returns the current reference count
		PE_NODISCARD RefCount GetRefCount() const;

	protected:
		Referenced();
		// Call automatically when reference count is zero
		// When called the object should cleanup/delete itself
		virtual void OnDrop() = 0;

	private:
		RefCount atomic_reference_count_;
	};
}
#endif // !PE_REFERENCED_H_
