#include "PxeVertexArray.h"

#include "PxeLogger.h"
#include "GL/glew.h"

namespace pxengine {
	PxeVertexArray::PxeVertexArray(bool delayAssetInitialization) : PxeGLAsset(delayAssetInitialization)
	{
		_glId = 0;
		_indexBuffer = nullptr;
		_bufferBindingsDirty = false;
		_bufferBindingError = false;
	}

	PxeVertexArray::~PxeVertexArray()
	{
		for (auto it = _bufferBindings.begin(); it != _bufferBindings.end(); ++it) {
			(*it).second.first->drop();
		}

		_bufferBindings.clear();
		if (_indexBuffer)
			_indexBuffer->drop();
	}

	void PxeVertexArray::bind()
	{
#ifdef PXE_DEBUG
		if (getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_WARN("Bound PxeVertexArray that's status was not INITIALIZED");
		}
#endif // PXE_DEBUG

		glBindVertexArray(_glId);
		if (_bufferBindingsDirty) {
			updateGlBindings();
		}
		else {
			if (_indexBuffer && _indexBuffer->getBufferPending())
				_indexBuffer->bind();
			for (auto pair : _bufferBindings) {
				if (pair.second.first->getBufferPending())
					pair.second.first->bind();
			}
		}
	}

	void PxeVertexArray::unbind()
	{
#ifdef PXE_DEBUG
		uint32_t previousBuffer;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (int32_t*)(&previousBuffer));
		if (previousBuffer != _glId) {
			PXE_WARN("unbind called on unbound PxeVertexArray");
		}
#endif // PXE_DEBUG
		glBindVertexArray(0);
	}

	void PxeVertexArray::updateBufferBindings()
	{
		_bufferBindingsDirty = true;
	}

	void PxeVertexArray::addVertexBuffer(PxeVertexBuffer& vertexBuffer, size_t vertexAttribIndex, uint8_t arrayIndex)
	{
		removeArrayAttrib(arrayIndex);
		vertexBuffer.grab();
		_bufferBindings.emplace(arrayIndex, std::pair(&vertexBuffer, vertexAttribIndex));
		_bufferBindingsDirty = true;
	}

	void PxeVertexArray::setIndexBuffer(PxeIndexBuffer* indexBuffer)
	{
		if (_indexBuffer) _indexBuffer->drop();
		if ((_indexBuffer = indexBuffer)) _indexBuffer->grab();
		_bufferBindingsDirty = true;
	}

	PXE_NODISCARD PxeIndexBuffer* PxeVertexArray::getIndexBuffer() const
	{
		return _indexBuffer;
	}

	void PxeVertexArray::removeArrayAttrib(uint8_t index)
	{
		auto it = _bufferBindings.find(index);
		if (it == _bufferBindings.end()) return;
		(*it).second.first->drop();
		_bufferBindings.erase(it);
		_bufferBindingsDirty = true;
	}

	PXE_NODISCARD bool PxeVertexArray::getAttribBound(uint8_t index, PxeVertexBuffer** vertexBuffer, size_t* vertexAttribIndex)
	{
		auto it = _bufferBindings.find(index);
		if (it == _bufferBindings.end()) return false;
		auto& binding = (*it).second;
		if (vertexBuffer)
			*vertexBuffer = binding.first;
		if (vertexAttribIndex)
			*vertexAttribIndex = binding.second;
		return true;
	}

	PXE_NODISCARD bool PxeVertexArray::getBindingError() const
	{
		return _bufferBindingError;
	}

	void PxeVertexArray::initializeGl()
	{
		glGenVertexArrays(1, &_glId);
		if (_glId == 0) {
			PXE_ERROR("Failed to create GlVertexArray");
			setErrorStatus();
			return;
		}
	}

	void PxeVertexArray::uninitializeGl()
	{
		if (_glId)
			glDeleteVertexArrays(1, &_glId);
	}

	void PxeVertexArray::updateGlBindings()
	{
		_bufferBindingsDirty = false;
		_bufferBindingError = false;
		for (auto binding : _bufferBindings)
		{
			PxeVertexBufferAttrib attrib;
			PxeVertexBuffer* buffer = binding.second.first;
			if (buffer && buffer->getAssetStatus() == PxeGLAssetStatus::INITIALIZED && buffer->getFormat().getAttrib(binding.second.second, attrib)) {
				buffer->bind();
				glVertexAttribPointer(binding.first, attrib.ComponentCount, (uint32_t)attrib.ComponentType, attrib.Normalized, buffer->getFormat().getStride(), (const void*)attrib.Offset);
				glEnableVertexAttribArray(binding.first);
			}
			else {
				glDisableVertexAttribArray(binding.first);
				_bufferBindingError = true;
			}
		}

		if (_indexBuffer)
			_indexBuffer->bind();
	}
}