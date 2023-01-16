#include "PxeBuffer.h"

namespace pxengine {
	PxeBuffer::PxeBuffer(const PxeBuffer& other)
	{
		_buffer = nullptr;
		_size = 0;
		(*this) = other;
	}

	PxeBuffer::PxeBuffer(size_t size)
	{
		_buffer = nullptr;
		_size = size;
		if (_size) {
			_buffer = malloc(_size);
			if (!_buffer)
				_size = 0;
		}
	}

	PxeBuffer::~PxeBuffer()
	{
		if (_buffer)
			free(_buffer);
	}

	PXE_NODISCARD bool PxeBuffer::getAllocated() const
	{
		return !!_buffer;
	}

	PxeBuffer& PxeBuffer::operator=(const PxeBuffer& other)
	{
		if (_buffer)
			free(_buffer);
		_buffer = nullptr;
		_size = other._size;
		if (_size) {
			_buffer = malloc(_size);
			if (!_buffer)
				_size = 0;
		}
		
		if (_size)
			memcpy(_buffer, other._buffer, _size);

		return *this;
	}

	PXE_NODISCARD size_t PxeBuffer::getSize() const {
		return _size;
	}

	PXE_NODISCARD void* PxeBuffer::getBuffer() const
	{
		return _buffer;
	}
}