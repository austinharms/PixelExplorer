#include "PxeBuffer.h"

namespace pxengine {
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
	DataType* PxeBuffer<DataType, LengthType>::getBuffer() const
	{
		return _buffer;
	}
}