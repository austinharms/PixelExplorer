#include "PxeIndexBuffer.h"

#include "GL/glew.h"
#include "nonpublic/NpLogger.h"

namespace pxengine {
	PxeIndexBuffer::PxeIndexBuffer(PxeIndexType indexType, PxeBuffer* buffer) : _glBufferType(indexType)
	{
		_glBufferId = 0;
		_currentBuffer = nullptr;
		_pendingBuffer = nullptr;
		if (buffer)
			bufferData(*buffer);
	}

	PxeIndexBuffer::~PxeIndexBuffer()
	{
		if (_pendingBuffer)
		{
			_pendingBuffer->drop();
			_pendingBuffer = nullptr;
		}

		// gl should be uninitialized and therefor there can not be a current buffer
		if (_currentBuffer) {
			PXE_ERROR("PxeIndexBuffer current buffer not deallocated before destructor call");
		}
	}

	void PxeIndexBuffer::bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
		if (_currentBuffer == nullptr) {
			PXE_WARN("Bound empty PxeIndexBuffer");
		}
	}

	void PxeIndexBuffer::unbind()
	{
#ifdef PXE_DEBUG
		uint32_t previousBuffer;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (int32_t*)(&previousBuffer));
		if (previousBuffer != _glBufferId) {
			PXE_WARN("unbind called on unbound PxeIndexBuffer");
		}
#endif // PXE_DEBUG

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void PxeIndexBuffer::bufferData(PxeBuffer& buffer)
	{
		buffer.grab();
		if (_pendingBuffer)
			_pendingBuffer->drop();
		_pendingBuffer = &buffer;
	}

	PxeBuffer* PxeIndexBuffer::getBuffer() const
	{
		return _currentBuffer;
	}

	PxeBuffer* PxeIndexBuffer::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	bool PxeIndexBuffer::getBufferPending() const
	{
		return _pendingBuffer;
	}

	uint32_t PxeIndexBuffer::getGlBufferId() const
	{
		return _glBufferId;
	}

	bool PxeIndexBuffer::getGlBufferValid() const
	{
		return _glBufferId;
	}

	PxeIndexType PxeIndexBuffer::getIndexType() const
	{
		return _glBufferType;
	}

	void PxeIndexBuffer::initializeGl()
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

	void PxeIndexBuffer::uninitializeGl()
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

	void PxeIndexBuffer::updateGlBuffer()
	{
		if (!getBufferPending() || !getGlBufferValid()) return;
		if (_currentBuffer)
			_currentBuffer->drop();
		_currentBuffer = _pendingBuffer;
		_pendingBuffer = nullptr;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _currentBuffer->getSize(), _currentBuffer->getBuffer(), GL_STATIC_DRAW);
	}
}