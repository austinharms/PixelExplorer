#include "PxeVertexArray.h"

#include "NpLogger.h"

namespace pxengine {
	PxeVertexArray::PxeVertexArray()
	{
		_glId = 0;
		_bufferBindingsDirty = false;
	}

	PxeVertexArray::~PxeVertexArray()
	{
	}

	void PxeVertexArray::bind()
	{
		if (!getValid()) {
			PXE_WARN("Attempted to bind invalid PxeVertexArray");
			return;
		}

		glBindVertexArray(_glId);
		if (_bufferBindingsDirty)
			updateBufferBindings();
	}

	void PxeVertexArray::unbind()
	{
		glBindVertexArray(0);
	}

	bool PxeVertexArray::getValid() const
	{
		return !_glId;
	}

	void PxeVertexArray::updateBufferBindings()
	{
		_bufferBindingsDirty = true;
	}

	void PxeVertexArray::addVertexBuffer(PxeVertexBuffer& vertexBuffer, size_t vertexAttribIndex, uint8_t arrayIndex)
	{
		removeArrayAttrib(arrayIndex);
		vertexBuffer.grab();
		_bufferBindings.emplace(arrayIndex, &vertexBuffer, vertexAttribIndex);
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

	bool PxeVertexArray::getAttribBound(uint8_t index, PxeVertexBuffer** vertexBuffer, size_t* vertexAttribIndex)
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
		glDeleteVertexArrays(1, &_glId);
	}

	void PxeVertexArray::updateGlBindings()
	{
		_bufferBindingsDirty = false;
		for (auto binding : _bufferBindings)
		{
			PxeVertexBufferAttrib attrib;
			if (binding.second.first && binding.second.first->getGlBufferValid() && binding.second.first->getFormat().getAttrib(binding.second.second, attrib)) {
				binding.second.first->bind();
				glVertexAttribPointer(binding.first, attrib.ComponentCount, (uint32_t)attrib.ComponentType, attrib.Normalized, attrib.Stride, (const void*)attrib.Offset);
				glEnableVertexAttribArray(binding.first);
				binding.second.first->unbind();
			}
			else {
				glDisableVertexAttribArray(binding.first);
			}
		}
	}
}