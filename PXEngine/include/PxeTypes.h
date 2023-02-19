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

	enum class PxeIndexType : uint32_t
	{
		UNDEFINED = 0,
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
	};

	typedef uint8_t PxeObjectFlagsType;
	enum class PxeObjectFlags : PxeObjectFlagsType
	{
		NONE = 0x00,
		RENDER_OBJECT = 0x01,
		PHYSICS_OBJECT = 0x02,
		PHYSICS_UPDATE = 0x04,
		ALL = 0x07,
	};

	typedef uint8_t PxeSceneUpdateFlagsType;
	enum class PxeSceneUpdateFlags : PxeSceneUpdateFlagsType
	{
		NONE = 0x00,
		PHYSICS_UPDATE = 0x01,
		ALL = 0x01
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

	enum class PxeFontScalingMode : uint8_t
	{
		UNSCALED = 0,	// Font is unscaled
		WIDTH,			// Font is scaled based on window width
		HEIGHT,			// Font is scaled based on window height
		SMALLEST,		// Font is scaled based on window width or height depending on what is smaller
		LARGEST			// Font is scaled based on window width or height depending on what is larger
	};

	enum class PxeFontStatus : uint8_t
	{
		UNDEFINED = 0,
		PENDING,
		LOADED,
		ERROR
	};

	typedef uint64_t PxeActionSourceCode;

	enum class PxeActionSourceType : uint8_t
	{
		NONE = 0,
		KEYBOARD,
		MOUSE,
		JOYSTICK_HAT,
		JOYSTICK_BUTTON,
		CONTROLLER_BUTTON,
	};

	enum class PxeRenderPass : uint8_t
	{
		DEFERED = 0,	// All lighting calculations are done on this pass
		FORWARD,
		TRANSPARENCY,
		SCREEN_SPACE,
		GUI,
		RENDER_PASS_COUNT
	};

	constexpr int32_t PxeRenderPassCount = (int32_t)PxeRenderPass::RENDER_PASS_COUNT;

	enum class PxeRenderMaterialType : uint8_t
	{
		UNDEFINED = 0,
		UNLIT,
	};
}

#define PXE_NODISCARD [[nodiscard]]
#endif // !PXENGINE_TYPES_H_
