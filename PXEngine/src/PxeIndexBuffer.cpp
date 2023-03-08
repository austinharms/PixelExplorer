#include "PxeIndexBuffer.h"

#include "GL/glew.h"
#include "PxeLogger.h"

namespace pxengine {
	size_t PxeIndexBuffer::getIndexSize(PxeIndexType type)
	{
		switch (type)
		{
		case pxengine::PxeIndexType::UNSIGNED_8BIT: return 1;
		case pxengine::PxeIndexType::UNSIGNED_16BIT: return 2;
		case pxengine::PxeIndexType::UNSIGNED_32BIT: return 4;
		}

		return 0;
	}

	PxeIndexBuffer::PxeIndexBuffer(PxeIndexType indexType, PxeBuffer* buffer, bool delayAssetInitialization) : PxeGLAsset(delayAssetInitialization), _glBufferType(indexType)
	{
		_glBufferId = 0;
		_pendingBuffer = nullptr;
		_indexCount = 0;
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
#ifdef PXE_DEBUG
		if (getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_WARN("Bound PxeIndexBuffer that's status was not INITIALIZED");
		}
#endif // PXE_DEBUG

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

	PXE_NODISCARD PxeIndexType PxeIndexBuffer::getIndexType() const
	{
		return _glBufferType;
	}

	PXE_NODISCARD uint32_t PxeIndexBuffer::getIndexCount() const
	{
		return _indexCount;
	}

	PXE_NODISCARD size_t PxeIndexBuffer::getIndexSize() const
	{
		return getIndexSize(_glBufferType);
	}

	void PxeIndexBuffer::initializeGl()
	{
		glGenBuffers(1, &_glBufferId);
		if (_glBufferId == 0) {
			PXE_ERROR("Failed to create Gl Element Array Buffer");
			setErrorStatus();
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
		if (!getBufferPending()) return;
#ifdef PXE_DEBUG
		if (_pendingBuffer->getSize() % getIndexSize(_glBufferType) != 0) {
			PXE_WARN("pending PxeBuffer size not divisible by size of PxeIndexBuffer's PxeIndexType");
		}
#endif // PXE_DEBUG

		_indexCount = static_cast<uint32_t>(_pendingBuffer->getSize() / getIndexSize(_glBufferType));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _pendingBuffer->getSize(), _pendingBuffer->getBuffer(), GL_STATIC_DRAW);
		_pendingBuffer->drop();
		_pendingBuffer = nullptr;
	}
}