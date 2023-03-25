#ifndef PXENGINE_FIXED_BUFFER_H_
#define PXENGINE_FIXED_BUFFER_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeBuffer.h"

namespace pxengine {
	// Helper class for storing and sharing continuous memory buffer of a fixed size
	class PxeFixedBuffer : public PxeBuffer
	{
	public:
		// Returns a new PxeFixedBuffer instance with buffer of {size} bytes or nullptr on failure
		// Note: the allocated buffer is uninitialized
		static PxeFixedBuffer* create(PxeSize size);

		// Returns a new PxeFixedBuffer instance that is a copy of {buffer} or nullptr on failure
		static PxeFixedBuffer* create(const PxeBuffer& buffer);

		// Returns the byte size of the allocated buffer
		PXE_NODISCARD virtual PxeSize getSize() const override;

		// Returns a pointer to the allocated memory
		// Note: this will return nullptr if the buffer size is 0
		PXE_NODISCARD virtual void* getBuffer() const override;

		virtual ~PxeFixedBuffer();
		PXE_NOCOPY(PxeFixedBuffer);

	private:
		PxeFixedBuffer(PxeSize size, void* buffer);
		const PxeSize _size;
		void* const _buffer;
	};
}
#endif // !PXENGINE_FIXED_BUFFER_H_
