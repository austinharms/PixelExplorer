#ifndef PXENGINE_BUFFER_H_
#define PXENGINE_BUFFER_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeBuffer : public PxeRefCount
	{
	public:
		// Returns a new PxeBuffer instance with buffer of {size} bytes or nullptr on failure
		// Note: the allocated buffer is uninitialized
		static PxeBuffer* create(PxeSize size);

		// Returns a new PxeBuffer instance that is a copy of {buffer} or nullptr on failure
		static PxeBuffer* create(const PxeBuffer& buffer);

		// Returns the byte size of the allocated buffer
		PXE_NODISCARD PxeSize getSize() const;

		// Returns a pointer to the allocated memory
		// Note: this will return nullptr if the buffer size is 0
		PXE_NODISCARD void* getBuffer() const;

		virtual ~PxeBuffer();
		PXE_NOCOPY(PxeBuffer);

	private:
		PxeBuffer(PxeSize size, void* buffer);
		const PxeSize _size;
		void* const _buffer;
	};
}
#endif // !PXENGINE_BUFFER_H_
