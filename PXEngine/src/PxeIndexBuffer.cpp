#include "PxeIndexBuffer.h"

#include "GL/glew.h"
#include "nonpublic/NpLogger.h"

namespace pxengine {
	PxeIndexBuffer::PxeIndexBuffer(PxeIndexType indexType, PxeBuffer* buffer) : _glBufferType(indexType)
	{
		_glBufferId = 0;
		_pendingBuffer = nullptr;
		if (buffer)
			bufferData(*buffer);
	}

	PxeIndexBuffer::~PxeIndexBuffer()
	{
		if (_pendingBuffer)
			_pendingBuffer->drop();
	}

	void PxeIndexBuffer::bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
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

	PXE_NODISCARD PxeBuffer* PxeIndexBuffer::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	PXE_NODISCARD bool PxeIndexBuffer::getBufferPending() const
	{
		return _pendingBuffer;
	}

	PXE_NODISCARD uint32_t PxeIndexBuffer::getGlBufferId() const
	{
		return _glBufferId;
	}

	PXE_NODISCARD bool PxeIndexBuffer::getValid() const
	{
		return _glBufferId;
	}

	PXE_NODISCARD PxeIndexType PxeIndexBuffer::getIndexType() const
	{
		return _glBufferType;
	}

	void PxeIndexBuffer::initializeGl()
	{
		glGenBuffers(1, &_glBufferId);
		if (!getValid()) {
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
		glDeleteBuffers(1, &_glBufferId);
	}

	void PxeIndexBuffer::updateGlBuffer()
	{
		if (!getBufferPending() || !getValid()) return;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _pendingBuffer->getSize(), _pendingBuffer->getBuffer(), GL_STATIC_DRAW);
		_pendingBuffer->drop();
		_pendingBuffer = nullptr;
	}
}