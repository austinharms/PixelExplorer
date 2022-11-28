#include "PxeGlVertexBuffer.h"

#include "GL/glew.h"
#include "NpLogger.h"

namespace pxengine {
	template<typename DataType, typename LengthType>
	PxeGlVertexBuffer<DataType, LengthType>::PxeGlVertexBuffer(PxeBufferType* buffer)
	{
		_glBufferId = 0;
		_currentBuffer = nullptr;
		_pendingBuffer = nullptr;
	}

	template<typename DataType, typename LengthType>
	PxeGlVertexBuffer<DataType, LengthType>::~PxeGlVertexBuffer()
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
	void PxeGlVertexBuffer<DataType, LengthType>::bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
		if (_currentBuffer == nullptr)
			PXE_WARN("Bound empty PxeGlVertexBuffer");
	}

	template<typename DataType, typename LengthType>
	void PxeGlVertexBuffer<DataType, LengthType>::unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template<typename DataType, typename LengthType>
	void PxeGlVertexBuffer<DataType, LengthType>::bufferData(PxeBufferType& buffer)
	{
		buffer.grab();
		if (_pendingBuffer)
			_pendingBuffer->drop();
		_pendingBuffer = &buffer;
	}

	template<typename DataType, typename LengthType>
	PxeGlVertexBuffer<DataType, LengthType>::PxeBufferType* PxeGlVertexBuffer<DataType, LengthType>::getBuffer() const
	{
		return _currentBuffer;
	}

	template<typename DataType, typename LengthType>
	PxeGlVertexBuffer<DataType, LengthType>::PxeBufferType* PxeGlVertexBuffer<DataType, LengthType>::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	template<typename DataType, typename LengthType>
	bool PxeGlVertexBuffer<DataType, LengthType>::getBufferPending() const
	{
		return _pendingBuffer != nullptr;
	}

	template<typename DataType, typename LengthType>
	uint32_t PxeGlVertexBuffer<DataType, LengthType>::getGlBufferId() const
	{
		return _glBufferId;
	}

	template<typename DataType, typename LengthType>
	bool PxeGlVertexBuffer<DataType, LengthType>::getGlBufferValid() const
	{
		return _glBufferId != 0;
	}

	template<typename DataType, typename LengthType>
	void PxeGlVertexBuffer<DataType, LengthType>::initializeGl()
	{
		glGenBuffers(1, &_glBufferId);
		if (!getGlBufferValid()) {
			PXE_ERROR("Failed to create GL_ARRAY_BUFFER buffer");
			return;
		}

		if (getBufferPending()) {
			// restores previously bound buffer
			uint32_t previousBuffer;
			glGetIntegerv(GL_ARRAY_BUFFER, &previousBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, _glBufferId);
			updateGlBuffer();
			glBindBuffer(GL_ARRAY_BUFFER, previousBuffer);
		}
	}

	template<typename DataType, typename LengthType>
	void PxeGlVertexBuffer<DataType, LengthType>::uninitializeGl()
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
	void PxeGlVertexBuffer<DataType, LengthType>::updateGlBuffer()
	{
		if (!getBufferPending() || !getGlBufferValid()) return;
		if (_currentBuffer)
			_currentBuffer->drop();
		_currentBuffer = _pendingBuffer;
		_pendingBuffer = nullptr;
		// TODO Buffer Data
	}
}