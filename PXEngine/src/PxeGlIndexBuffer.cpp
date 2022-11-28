#include "PxeGlIndexBuffer.h"

#include "GL/glew.h"
#include "NpLogger.h"

namespace pxengine {
	template<typename DataType, typename LengthType>
	PxeGlIndexBuffer<DataType, LengthType>::PxeGlIndexBuffer(PxeBufferType* buffer)
	{
		_glBufferId = 0;
		_currentBuffer = nullptr;
		_pendingBuffer = nullptr;
	}

	template<typename DataType, typename LengthType>
	PxeGlIndexBuffer<DataType, LengthType>::~PxeGlIndexBuffer()
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
	void PxeGlIndexBuffer<DataType, LengthType>::bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
		if (_currentBuffer == nullptr)
			PXE_WARN("Bound empty PxeGlIndexBuffer");
	}

	template<typename DataType, typename LengthType>
	void PxeGlIndexBuffer<DataType, LengthType>::unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	template<typename DataType, typename LengthType>
	void PxeGlIndexBuffer<DataType, LengthType>::bufferData(PxeBufferType& buffer)
	{
		buffer.grab();
		if (_pendingBuffer)
			_pendingBuffer->drop();
		_pendingBuffer = &buffer;
	}

	template<typename DataType, typename LengthType>
	PxeGlIndexBuffer<DataType, LengthType>::PxeBufferType* PxeGlIndexBuffer<DataType, LengthType>::getBuffer() const
	{
		return _currentBuffer;
	}

	template<typename DataType, typename LengthType>
	PxeGlIndexBuffer<DataType, LengthType>::PxeBufferType* PxeGlIndexBuffer<DataType, LengthType>::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	template<typename DataType, typename LengthType>
	bool PxeGlIndexBuffer<DataType, LengthType>::getBufferPending() const
	{
		return _pendingBuffer != nullptr;
	}

	template<typename DataType, typename LengthType>
	uint32_t PxeGlIndexBuffer<DataType, LengthType>::getGlBufferId() const
	{
		return _glBufferId;
	}

	template<typename DataType, typename LengthType>
	bool PxeGlIndexBuffer<DataType, LengthType>::getGlBufferValid() const
	{
		return _glBufferId != 0;
	}

	template<typename DataType, typename LengthType>
	void PxeGlIndexBuffer<DataType, LengthType>::initializeGl()
	{
		glGenBuffers(1, &_glBufferId);
		if (!getGlBufferValid()) {
			PXE_ERROR("Failed to create GL_ELEMENT_ARRAY_BUFFER buffer");
			return;
		}

		if (getBufferPending()) {
			// restores previously bound buffer
			uint32_t previousBuffer;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER, &previousBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBufferId);
			updateGlBuffer();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previousBuffer);
		}
	}

	template<typename DataType, typename LengthType>
	void PxeGlIndexBuffer<DataType, LengthType>::uninitializeGl()
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
	void PxeGlIndexBuffer<DataType, LengthType>::updateGlBuffer()
	{
		if (!getBufferPending() || !getGlBufferValid()) return;
		if (_currentBuffer)
			_currentBuffer->drop();
		_currentBuffer = _pendingBuffer;
		_pendingBuffer = nullptr;
		// TODO Buffer Data
	}
}