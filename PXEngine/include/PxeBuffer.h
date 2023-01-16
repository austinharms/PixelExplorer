#ifndef PXENGINE_BUFFER_H_
#define PXENGINE_BUFFER_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeBuffer : public PxeRefCount
	{
	public:
		// Allocates a buffer with a byte length of {size}
		PxeBuffer(size_t size);
		PxeBuffer(const PxeBuffer& other);
		virtual ~PxeBuffer();
		// Note: a new buffer allocated and all data is copied
		PxeBuffer& operator=(const PxeBuffer& other);

		// Returns true if the buffer is allocated and false on failure
		PXE_NODISCARD bool getAllocated() const;

		// Returns the byte size of the buffer
		// Note: if there was an allocation error this will return 0
		PXE_NODISCARD size_t getSize() const;

		// Returns a pointer to the internal buffer
		// Note: return value may be nullptr if there was an allocation error
		PXE_NODISCARD void* getBuffer() const;

	private:
		void* _buffer;
		size_t _size;
	};
}
#endif // !PXENGINE_BUFFER_H_
