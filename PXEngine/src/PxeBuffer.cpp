#include "PxeBuffer.h"

#include <new>
#include <malloc.h>

#include "PxeLogger.h"

namespace pxengine {
	PxeBuffer* PxeBuffer::create(PxeSize size)
	{
		void* buf = nullptr;
		if (size) {
			buf = malloc(size);
			if (!buf) {
				PXE_ERROR("Failed to allocate PxeBuffer's buffer");
				return nullptr;
			}
		}

		PxeBuffer* pxeBuf = new(std::nothrow) PxeBuffer(size, buf);
		if (!pxeBuf) {
			PXE_ERROR("Failed to allocate PxeBuffer");
			if (buf) free(buf);
		}

		return pxeBuf;
	}

	PxeBuffer* PxeBuffer::create(const PxeBuffer& buffer)
	{
		PxeBuffer* pxeBuf = create(buffer._size);
		if (pxeBuf && buffer._size)
			memcpy(pxeBuf->_buffer, buffer._buffer, buffer._size);
		return pxeBuf;
	}

	PxeBuffer::PxeBuffer(PxeSize size, void* buffer) : _buffer(buffer), _size(size) {}

	PxeBuffer::~PxeBuffer()
	{
		if (_buffer) free(_buffer);
	}

	PXE_NODISCARD PxeSize PxeBuffer::getSize() const {
		return _size;
	}

	PXE_NODISCARD void* PxeBuffer::getBuffer() const
	{
		return _buffer;
	}
}