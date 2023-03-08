#ifndef PXENGINE_VERTEXBUFFERATTRIB_H_
#define PXENGINE_VERTEXBUFFERATTRIB_H_
#include "PxeTypes.h"

namespace pxengine {
	// Struct used to store glAttrib information 
	struct PxeVertexBufferAttrib
	{
	public:
		PxeVertexBufferAttribType ComponentType;
		uint8_t ComponentCount;
		bool Normalized;
		// Note: this is not a size_t because glVertexAttribPointer only supports a GLuint
		uint32_t Offset;

		// Returns the byte size of the PxeVertexBufferAttribType {t}
		// Note: the return is not a size_t because glVertexAttribPointer only supports a GLuint
		static PXE_NODISCARD uint32_t componentWidth(PxeVertexBufferAttribType t) {
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

		// Returns the byte size of the PxeVertexBufferAttrib ComponentType
		// Note: the return is not a size_t because glVertexAttribPointer only supports a GLuint
		inline PXE_NODISCARD uint32_t getComponentWidth() const { return componentWidth(ComponentType); }

		// Returns the byte size of the PxeVertexBufferAttrib
		// Note: this is NOT equivalent to sizeof(PxeVertexBufferAttrib)
		// Note: the return is not a size_t because glVertexAttribPointer only supports a GLuint
		inline PXE_NODISCARD uint32_t getByteWidth() const { return getComponentWidth() * ComponentCount; }
	};
}
#endif