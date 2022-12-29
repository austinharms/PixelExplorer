#include "PxeVertexBuffer.h"

#include "GL/glew.h"
#include "nonpublic/NpLogger.h"

namespace pxengine {
	PxeVertexBuffer::PxeVertexBuffer(const PxeVertexBufferFormat& bufferFormat, PxeBuffer* buffer)
	{
		_glBufferId = 0;
		_currentBuffer = nullptr;
		_pendingBuffer = nullptr;
		setFormat(bufferFormat);
		if (buffer)
			bufferData(*buffer);
	}

	PxeVertexBuffer::~PxeVertexBuffer()
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

	void PxeVertexBuffer::bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
		if (_currentBuffer == nullptr)
			PXE_WARN("Bound empty PxeGlVertexBuffer");
	}

	void PxeVertexBuffer::unbind()
	{
#ifdef PXE_DEBUG
		uint32_t previousBuffer;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (int32_t*)(&previousBuffer));
		if (previousBuffer != _glBufferId) {
			PXE_WARN("unbind called on unbound PxeVertexBuffer");
		}
#endif // PXE_DEBUG
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void PxeVertexBuffer::bufferData(PxeBuffer& buffer)
	{
		buffer.grab();
		if (_pendingBuffer)
			_pendingBuffer->drop();
		_pendingBuffer = &buffer;
	}

	PxeBuffer* PxeVertexBuffer::getBuffer() const
	{
		return _currentBuffer;
	}

	PxeBuffer* PxeVertexBuffer::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	bool PxeVertexBuffer::getBufferPending() const
	{
		return _pendingBuffer != nullptr;
	}

	uint32_t PxeVertexBuffer::getGlBufferId() const
	{
		return _glBufferId;
	}

	bool PxeVertexBuffer::getGlBufferValid() const
	{
		return _glBufferId;
	}

	const PxeVertexBufferFormat& PxeVertexBuffer::getFormat() const
	{
		return _format;
	}

	void PxeVertexBuffer::setFormat(const PxeVertexBufferFormat& format)
	{
		_format = format;
	}

	void PxeVertexBuffer::initializeGl()
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

	void PxeVertexBuffer::uninitializeGl()
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

	void PxeVertexBuffer::updateGlBuffer()
	{
		if (!getBufferPending() || !getGlBufferValid()) return;
		if (_currentBuffer)
			_currentBuffer->drop();
		_currentBuffer = _pendingBuffer;
		_pendingBuffer = nullptr;
		if (_currentBuffer->getSize() % _format.getBufferStride() != 0)
			PXE_WARN("Buffer data not divisible by buffer stride");
		glBufferData(GL_ARRAY_BUFFER, _currentBuffer->getSize(), _currentBuffer->getBuffer(), GL_STATIC_DRAW);
	}
}