#ifndef PXENGINE_BUFFER_H_
#define PXENGINE_BUFFER_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeBuffer : public PxeRefCount
	{
	public:
		// Returns true if the buffer is allocated and false on failure
		PXE_NODISCARD bool getAllocated() const;

		// Returns the byte size of the buffer
		// Note: if there was an allocation error this will return 0
		PXE_NODISCARD PxeSize getSize() const;

		// Returns a pointer to the internal buffer
		// Note: return value may be nullptr if there was an allocation error
		PXE_NODISCARD void* getBuffer() const;

		// Allocates a buffer with a byte length of {size}
		// Note: the buffer is not initialized to 0's
		PxeBuffer(size_t size);

		// Copy buffer data
		PxeBuffer(const PxeBuffer& other);

		// Copy buffer data
		// Note: a new block of memory will be allocated and the old one freed
		PxeBuffer& operator=(const PxeBuffer& other);

		virtual ~PxeBuffer();
	private:
		PxeSize _size;
		void* _buffer;
	};
}
#endif // !PXENGINE_BUFFER_H_
