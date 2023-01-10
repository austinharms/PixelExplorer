#include <stdint.h>

#include "GL/glew.h"

#ifndef PXENGINE_VERTEXBUFFERATTRIB_H_
#define PXENGINE_VERTEXBUFFERATTRIB_H_
namespace pxengine {
	enum class PxeVertexBufferAttribType : uint32_t
	{
		UNDEFINED = 0,
		BYTE = GL_BYTE,
		UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		UNSIGNED_INT = GL_UNSIGNED_INT,
		HALF_FLOAT = GL_HALF_FLOAT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,
	};

	struct PxeVertexBufferAttrib
	{
	public:

		static size_t componentWidth(PxeVertexBufferAttribType t) {
			switch (t)
			{
			case PxeVertexBufferAttribType::BYTE:
			case PxeVertexBufferAttribType::UNSIGNED_BYTE:
				return 1;
			case PxeVertexBufferAttribType::SHORT:
			case PxeVertexBufferAttribType::UNSIGNED_SHORT:
			case PxeVertexBufferAttribType::HALF_FLOAT:
				return 2;
			case PxeVertexBufferAttribType::INT:
			case PxeVertexBufferAttribType::UNSIGNED_INT:
			case PxeVertexBufferAttribType::FLOAT:
				return 4;
			case PxeVertexBufferAttribType::DOUBLE:
				return 8;
			default:
				return 0;
			}
		}

		PxeVertexBufferAttribType ComponentType;
		uint8_t ComponentCount;
		bool Normalized;
		uint32_t Offset;
		uint32_t Stride;

		PxeVertexBufferAttrib() {
			ComponentType = PxeVertexBufferAttribType::UNDEFINED;
			ComponentCount = 0;
			Normalized  = false;
			Offset = 0;
			Stride = 0;
		}

		PxeVertexBufferAttrib(PxeVertexBufferAttribType type, uint8_t count, bool normalized) {
			ComponentType = type;
			ComponentCount = count;
			Normalized = normalized;
			Offset = 0;
			Stride = 0;
		}

		PxeVertexBufferAttrib(const PxeVertexBufferAttrib& other) {
			ComponentType = other.ComponentType;
			ComponentCount = other.ComponentCount;
			Normalized = other.Normalized;
			Offset = other.Offset;
			Stride = other.Stride;
		}

		PxeVertexBufferAttrib& operator=(const PxeVertexBufferAttrib& other) {
			ComponentType = other.ComponentType;
			ComponentCount = other.ComponentCount;
			Normalized = other.Normalized;
			Offset = other.Offset;
			Stride = other.Stride;
			return *this;
		}

		size_t getComponentByteWidth() const {
			return componentWidth(ComponentType);
		}

		size_t getByteWidth() const {
			return getComponentByteWidth() * ComponentCount;
		}
	};
}
#endif