#include "PxeVertexBuffer.h"

#include "GL/glew.h"
#include "NpLogger.h"

namespace pxengine {
	template<typename DataType, typename LengthType>
	PxeVertexBuffer<DataType, LengthType>::PxeVertexBuffer(PxeBufferType* buffer)
	{
		_glBufferId = 0;
		_currentBuffer = nullptr;
		_pendingBuffer = nullptr;
	}

	template<typename DataType, typename LengthType>
	PxeVertexBuffer<DataType, LengthType>::~PxeVertexBuffer()
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
	void PxeVertexBuffer<DataType, LengthType>::bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
		if (_currentBuffer == nullptr)
			PXE_WARN("Bound empty PxeGlVertexBuffer");
	}

	template<typename DataType, typename LengthType>
	void PxeVertexBuffer<DataType, LengthType>::unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template<typename DataType, typename LengthType>
	void PxeVertexBuffer<DataType, LengthType>::bufferData(PxeBufferType& buffer)
	{
		buffer.grab();
		if (_pendingBuffer)
			_pendingBuffer->drop();
		_pendingBuffer = &buffer;
	}

	template<typename DataType, typename LengthType>
	PxeVertexBuffer<DataType, LengthType>::PxeBufferType* PxeVertexBuffer<DataType, LengthType>::getBuffer() const
	{
		return _currentBuffer;
	}

	template<typename DataType, typename LengthType>
	PxeVertexBuffer<DataType, LengthType>::PxeBufferType* PxeVertexBuffer<DataType, LengthType>::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	template<typename DataType, typename LengthType>
	bool PxeVertexBuffer<DataType, LengthType>::getBufferPending() const
	{
		return _pendingBuffer != nullptr;
	}

	template<typename DataType, typename LengthType>
	uint32_t PxeVertexBuffer<DataType, LengthType>::getGlBufferId() const
	{
		return _glBufferId;
	}

	template<typename DataType, typename LengthType>
	bool PxeVertexBuffer<DataType, LengthType>::getGlBufferValid() const
	{
		return _glBufferId != 0;
	}

	template<typename DataType, typename LengthType>
	void PxeVertexBuffer<DataType, LengthType>::initializeGl()
	{
		glGenBuffers(1, &_glBufferId);
		if (!getGlBufferValid()) {
			PXE_ERROR("Failed to create GL_ARRAY_BUFFER buffer");
			return;
		}

		if (getBufferPending()) {
			// restores previously bound buffer
			uint32_t previousBuffer;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (int32_t*)(&previousBuffer));
			glBindBuffer(GL_ARRAY_BUFFER, _glBufferId);
			updateGlBuffer();
			glBindBuffer(GL_ARRAY_BUFFER, previousBuffer);
		}
	}

	template<typename DataType, typename LengthType>
	void PxeVertexBuffer<DataType, LengthType>::uninitializeGl()
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
	void PxeVertexBuffer<DataType, LengthType>::updateGlBuffer()
	{
		if (!getBufferPending() || !getGlBufferValid()) return;
		if (_currentBuffer)
			_currentBuffer->drop();
		_currentBuffer = _pendingBuffer;
		_pendingBuffer = nullptr;
		glBufferData(GL_ARRAY_BUFFER, _currentBuffer->getByteSize(), _currentBuffer->getBuffer(), GL_STATIC_DRAW);
	}
}