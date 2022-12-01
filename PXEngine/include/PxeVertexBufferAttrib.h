#include <stdint.h>

#include "gl/GL.h"

#ifndef PXENGINE_VERTEXBUFFERATTRIB_H_
#define PXENGINE_VERTEXBUFFERATTRIB_H_
namespace pxengine {
	struct PxeVertexBufferAttrib
	{
	public:
		uint32_t ComponentType;
		uint8_t ComponentCount;
		bool Normalized;
	};
}
#endif