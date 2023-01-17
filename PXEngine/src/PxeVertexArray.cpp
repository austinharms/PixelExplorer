#include "PxeVertexArray.h"

#include "nonpublic/NpLogger.h"

namespace pxengine {
	PxeVertexArray::PxeVertexArray()
	{
		_glId = 0;
		_bufferBindingsDirty = false;
	}

	PxeVertexArray::~PxeVertexArray()
	{
		for (auto it = _bufferBindings.begin(); it != _bufferBindings.end(); ++it) {
			(*it).second.first->drop();
		}

		_bufferBindings.clear();
	}

	void PxeVertexArray::bind()
	{
#ifdef PXE_DEBUG
		if (getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_WARN("Bound PxeVertexArray that's status was not INITIALIZED");
		}
#endif // PXE_DEBUG

		glBindVertexArray(_glId);
		if (_bufferBindingsDirty)
			updateBufferBindings();
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

	PXE_NODISCARD bool PxeVertexArray::getValid() const
	{
		return _glId;
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

	void PxeVertexArray::initializeGl()
	{
		glGenVertexArrays(1, &_glId);
		if (!getValid()) {
			PXE_ERROR("Failed to create GLVertexArray");
			setErrorStatus();
			return;
		}

		uint32_t previousBuffer;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (int32_t*)(&previousBuffer));
		glBindVertexArray(_glId);
		updateGlBindings();
		glBindVertexArray(previousBuffer);
	}

	void PxeVertexArray::uninitializeGl()
	{
		if (_glId)
			glDeleteVertexArrays(1, &_glId);
	}

	void PxeVertexArray::updateGlBindings()
	{
		_bufferBindingsDirty = false;
		for (auto binding : _bufferBindings)
		{
			PxeVertexBufferAttrib attrib;
			PxeVertexBuffer* buffer = binding.second.first;
			if (buffer && buffer->getValid() && buffer->getFormat().getAttrib(binding.second.second, attrib)) {
				buffer->bind();
				glVertexAttribPointer(binding.first, attrib.ComponentCount, (uint32_t)attrib.ComponentType, attrib.Normalized, buffer->getFormat().getStride(), (const void*)attrib.Offset);
				glEnableVertexAttribArray(binding.first);
				buffer->unbind();
			}
			else {
				glDisableVertexAttribArray(binding.first);
			}
		}
	}
}