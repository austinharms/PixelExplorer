#include "PxeFixedBuffer.h"

#include <new>
#include <malloc.h>

#include "PxeLogger.h"

namespace pxengine {
	PxeFixedBuffer* PxeFixedBuffer::create(PxeSize size)
	{
		void* buf = nullptr;
		if (size) {
			buf = malloc(size);
			if (!buf) {
				PXE_ERROR("Failed to allocate PxeFixedBuffer's buffer");
				return nullptr;
			}
		}

		PxeFixedBuffer* pxeBuf = new(std::nothrow) PxeFixedBuffer(size, buf);
		if (!pxeBuf) {
			PXE_ERROR("Failed to allocate PxeFixedBuffer");
			if (buf) free(buf);
		}

		return pxeBuf;
	}

	PxeFixedBuffer* PxeFixedBuffer::create(const PxeBuffer& buffer)
	{
		PxeFixedBuffer* pxeBuf = create(buffer.getSize());
		if (pxeBuf && pxeBuf->_size)
			memcpy(pxeBuf->_buffer, buffer.getBuffer(), pxeBuf->_size);
		return pxeBuf;
	}

	PxeFixedBuffer::PxeFixedBuffer(PxeSize size, void* buffer) : _buffer(buffer), _size(size) {}

	PxeFixedBuffer::~PxeFixedBuffer()
	{
		if (_buffer) free(_buffer);
	}

	PXE_NODISCARD PxeSize PxeFixedBuffer::getSize() const {
		return _size;
	}

	PXE_NODISCARD void* PxeFixedBuffer::getBuffer() const
	{
		return _buffer;
	}
}