#include "PxeIndexBuffer.h"

#include "GL/glew.h"
#include "NpLogger.h"

namespace pxengine {
	template<typename DataType, typename LengthType>
	PxeIndexBuffer<DataType, LengthType>::PxeIndexBuffer(PxeBufferType* buffer)
	{
		_glBufferId = 0;
		_currentBuffer = nullptr;
		_pendingBuffer = nullptr;
	}

	template<typename DataType, typename LengthType>
	PxeIndexBuffer<DataType, LengthType>::~PxeIndexBuffer()
	{
		if (_currentBuffer) {
			_currentBuffer->drop();
			_currentBuffer = nullptr;
		}

		if (_pendingBuffer) {
			_pendingBuffer->drop();
			_pendingBuffer = nullptr;
		}

		_glBufferId = 0;
	}

	template<typename DataType, typename LengthType>
	void PxeIndexBuffer<DataType, LengthType>::bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
		if (_currentBuffer == nullptr)
			PXE_WARN("Bound empty PxeIndexBuffer");
	}

	template<typename DataType, typename LengthType>
	void PxeIndexBuffer<DataType, LengthType>::unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	template<typename DataType, typename LengthType>
	void PxeIndexBuffer<DataType, LengthType>::bufferData(PxeBufferType& buffer)
	{
		buffer.grab();
		if (_pendingBuffer)
			_pendingBuffer->drop();
		_pendingBuffer = &buffer;
	}

	template<typename DataType, typename LengthType>
	PxeIndexBuffer<DataType, LengthType>::PxeBufferType* PxeIndexBuffer<DataType, LengthType>::getBuffer() const
	{
		return _currentBuffer;
	}

	template<typename DataType, typename LengthType>
	PxeIndexBuffer<DataType, LengthType>::PxeBufferType* PxeIndexBuffer<DataType, LengthType>::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	template<typename DataType, typename LengthType>
	bool PxeIndexBuffer<DataType, LengthType>::getBufferPending() const
	{
		return _pendingBuffer != nullptr;
	}

	template<typename DataType, typename LengthType>
	uint32_t PxeIndexBuffer<DataType, LengthType>::getGlBufferId() const
	{
		return _glBufferId;
	}

	template<typename DataType, typename LengthType>
	bool PxeIndexBuffer<DataType, LengthType>::getGlBufferValid() const
	{
		return _glBufferId != 0;
	}

	template<typename DataType, typename LengthType>
	void PxeIndexBuffer<DataType, LengthType>::initializeGl()
	{
		glGenBuffers(1, &_glBufferId);
		if (!getGlBufferValid()) {
			PXE_ERROR("Failed to create GL_ELEMENT_ARRAY_BUFFER buffer");
			return;
		}

		if (getBufferPending()) {
			// restores previously bound buffer
			uint32_t previousBuffer;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (int32_t*)(&previousBuffer));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBufferId);
			updateGlBuffer();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previousBuffer);
		}
	}

	template<typename DataType, typename LengthType>
	void PxeIndexBuffer<DataType, LengthType>::uninitializeGl()
	{
		if (_pendingBuffer) {
			_currentBuffer->drop();
			_currentBuffer = nullptr;
		}
		else {
			_pendingBuffer = _currentBuffer;
			_currentBuffer = nullptr;
		}

		glDeleteBuffers(1, &_glBufferId);
	}

	template<typename DataType, typename LengthType>
	void PxeIndexBuffer<DataType, LengthType>::updateGlBuffer()
	{
		if (!getBufferPending() || !getGlBufferValid()) return;
		if (_currentBuffer)
			_currentBuffer->drop();
		_currentBuffer = _pendingBuffer;
		_pendingBuffer = nullptr;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _currentBuffer->getByteSize(), _currentBuffer->getBuffer(), GL_STATIC_DRAW);
	}
}