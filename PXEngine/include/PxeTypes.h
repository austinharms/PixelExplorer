#ifndef PXENGINE_TYPES_H_
#define PXENGINE_TYPES_H_
// Sized int types
#include <stdint.h>

// GLM types
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat2x3.hpp"
#include "glm/mat3x2.hpp"
#include "glm/mat2x4.hpp"
#include "glm/mat4x2.hpp"
#include "glm/mat3x4.hpp"
#include "glm/mat4x3.hpp"

namespace pxengine {
	enum class PxeGLAssetStatus : uint8_t
	{
		UNINITIALIZED = 0,
		UNINITIALIZING,
		PENDING_UNINITIALIZATION,
		PENDING_INITIALIZATION,
		INITIALIZING,
		INITIALIZED,
		ERROR,
	};

	enum class PxeWindowProjection
	{
		NONE = 0,
		ORTHOGRAPHIC,
		PERSPECTIVE,
		CUSTOM,
	};

	enum class PxeIndexType : uint32_t
	{
		UNSIGNED_8BIT = 0x1401,  // GL_UNSIGNED_BYTE
		UNSIGNED_16BIT = 0x1403, // GL_UNSIGNED_SHORT
		UNSIGNED_32BIT = 0x1405, // GL_UNSIGNED_INT
	};

	enum class PxeLogLevel : uint8_t
	{
		PXE_INFO = 0,
		PXE_WARN,
		PXE_ERROR,
		PXE_FATAL,
		PXE_LOGLEVELCOUNT
	};

	enum class PxeRenderPass : int8_t
	{
		NONE = -1,
		SCREEN_SPACE,
		WORLD_SPACE,
		RENDER_PASS_COUNT
	};

	enum class PxeVertexBufferAttribType : uint32_t
	{
		UNDEFINED = 0,
		BYTE = 0x1400,				// GL_BYTE
		UNSIGNED_BYTE = 0x1401,		// GL_UNSIGNED_BYTE
		SHORT = 0x1402,				// GL_SHORT
		UNSIGNED_SHORT = 0x1403,	// GL_UNSIGNED_SHORT
		INT = 0x1404,				// GL_INT
		UNSIGNED_INT = 0x1405,		// GL_UNSIGNED_INT
		HALF_FLOAT = 0x140B,		// GL_HALF_FLOAT
		FLOAT = 0x1406,				// GL_FLOAT
		DOUBLE = 0x140A,			// GL_DOUBLE
	};
}

#define PXE_NODISCARD [[nodiscard]]
#endif // !PXENGINE_TYPES_H_
