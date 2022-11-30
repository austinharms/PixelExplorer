#include "PxeBuffer.h"

namespace pxengine {
	template<typename DataType, typename LengthType>
	PxeBuffer<DataType, LengthType>::PxeBuffer(const PxeBuffer& other)
	{
		(*this) = other;
	}

	template<typename DataType, typename LengthType>
	PxeBuffer<DataType, LengthType>::PxeBuffer(LengthType size)
	{
		_buffer = malloc(size * sizeof(DataType));
	}

	template<typename DataType, typename LengthType>
	PxeBuffer<DataType, LengthType>::~PxeBuffer()
	{
		if (_buffer)
			free(_buffer);
	}

	template<typename DataType, typename LengthType>
	bool PxeBuffer<DataType, LengthType>::getAllocated() const
	{
		return _buffer != nullptr;
	}

	template<typename DataType, typename LengthType>
	LengthType PxeBuffer<DataType, LengthType>::getLength() const
	{
		return _length;
	}

	template<typename DataType, typename LengthType>
	size_t PxeBuffer<DataType, LengthType>::getByteSize() const
	{
		return getTypeSize() * _length;
	}

	template<typename DataType, typename LengthType>
	PxeBuffer<DataType, LengthType>& PxeBuffer<DataType, LengthType>::operator=(const PxeBuffer<DataType, LengthType>& other)
	{
		_length = other._length;
		_buffer = malloc(other.getByteSize());
		memcpy(_buffer, other._buffer, getByteSize());
		return *this;
	}

	template<typename DataType, typename LengthType>
	DataType* PxeBuffer<DataType, LengthType>::getBuffer() const
	{
		return _buffer;
	}
}