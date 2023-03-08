#include "PxeVertexBuffer.h"

#include "GL/glew.h"
#include "PxeLogger.h"

namespace pxengine {
	PxeVertexBuffer::PxeVertexBuffer(const PxeVertexBufferFormat& bufferFormat, PxeBuffer* buffer, bool delayAssetInitialization) : PxeGLAsset(delayAssetInitialization)
	{
		_glBufferId = 0;
		_pendingBuffer = nullptr;
		setFormat(bufferFormat);
		if (buffer)
			bufferData(*buffer);
	}

	PxeVertexBuffer::~PxeVertexBuffer()
	{
		if (_pendingBuffer)
			_pendingBuffer->drop();
	}

	void PxeVertexBuffer::bind()
	{
#ifdef PXE_DEBUG
		if (getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_WARN("Bound PxeVertexBuffer that's status was not INITIALIZED");
		}
#endif // PXE_DEBUG


		glBindBuffer(GL_ARRAY_BUFFER, _glBufferId);
		updateGlBuffer();
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

	PXE_NODISCARD PxeBuffer* PxeVertexBuffer::getPendingBuffer() const
	{
		return _pendingBuffer;
	}

	PXE_NODISCARD bool PxeVertexBuffer::getBufferPending() const
	{
		return _pendingBuffer != nullptr;
	}

	PXE_NODISCARD uint32_t PxeVertexBuffer::getGlBufferId() const
	{
		return _glBufferId;
	}

	PXE_NODISCARD const PxeVertexBufferFormat& PxeVertexBuffer::getFormat() const
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
		if (_glBufferId == 0) {
			PXE_ERROR("Failed to create Gl Array Buffer");
			setErrorStatus();
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
		glDeleteBuffers(1, &_glBufferId);
	}

	void PxeVertexBuffer::updateGlBuffer()
	{
		if (!getBufferPending()) return;
#ifdef PXE_DEBUG
		if (_pendingBuffer->getSize() % _format.getStride() != 0) {
			PXE_WARN("Buffer data not divisible by buffer stride");
		}
#endif // PXE_DEBUG
		glBufferData(GL_ARRAY_BUFFER, _pendingBuffer->getSize(), _pendingBuffer->getBuffer(), GL_STATIC_DRAW);
		_pendingBuffer->drop();
		_pendingBuffer = nullptr;
	}
}