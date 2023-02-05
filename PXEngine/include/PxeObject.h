#ifndef PXENGINE_OBJECT_H_
#define PXENGINE_OBJECT_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	// Base class for all PxeScene objects
	class PxeObject : public PxeRefCount
	{
	public:
		inline PxeObject(PxeObjectFlags flags = PxeObjectFlags::NONE) : _flags((PxeObjectFlagsType)flags) {}
		virtual ~PxeObject() = default;

		// Returns the object's PxeUpdateFlags
		inline PXE_NODISCARD PxeObjectFlagsType getObjectFlags() const { return _flags; }

		PxeObject(const PxeObject& other) = delete;
		PxeObject operator=(const PxeObject& other) = delete;

	private:
		const PxeObjectFlagsType _flags;
	};
}
#endif // !PXENGINE_OBJECT_H_