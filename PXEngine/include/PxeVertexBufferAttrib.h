#include <stdint.h>

#include "GL/glew.h"

#ifndef PXENGINE_VERTEXBUFFERATTRIB_H_
#define PXENGINE_VERTEXBUFFERATTRIB_H_
namespace pxengine {
	struct PxeVertexBufferAttrib
	{
	public:
		enum class AttribType : uint32_t
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

		static size_t componentWidth(AttribType t) {
			switch (t)
			{
			case AttribType::BYTE:
			case AttribType::UNSIGNED_BYTE:
				return 1;
			case AttribType::SHORT:
			case AttribType::UNSIGNED_SHORT:
			case AttribType::HALF_FLOAT:
				return 2;
			case AttribType::INT:
			case AttribType::UNSIGNED_INT:
			case AttribType::FLOAT:
				return 4;
			case AttribType::DOUBLE:
				return 8;
			default:
				return 0;
			}
		}

		AttribType ComponentType;
		uint8_t ComponentCount;
		bool Normalized;
		uint32_t Offset;
		uint32_t Stride;

		PxeVertexBufferAttrib() {
			ComponentType = AttribType::UNDEFINED;
			ComponentCount = 0;
			Normalized  = false;
			Offset = 0;
			Stride = 0;
		}

		PxeVertexBufferAttrib(AttribType type, uint8_t count, bool normalized) {
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