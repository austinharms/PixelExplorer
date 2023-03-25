#ifndef PXENGINE_BUFFER_H_
#define PXENGINE_BUFFER_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	// Helper interface class for storing and sharing continuous blocks of memory
	class PxeBuffer : public PxeRefCount
	{
	public:
		// Returns the byte size of the allocated buffer
		PXE_NODISCARD virtual PxeSize getSize() const = 0;

		// Returns a pointer to the allocated memory
		// Note: this will return nullptr if the buffer size is 0
		PXE_NODISCARD virtual void* getBuffer() const = 0;

		virtual ~PxeBuffer() = default;
		PXE_NOCOPY(PxeBuffer);

	protected:
		PxeBuffer() = default;
	};
}
#endif // !PXENGINE_BUFFER_H_
