#ifndef PXENGINE_TYPES_H_
#define PXENGINE_TYPES_H_
// Sized integer types
#include <stdint.h>

// Basic GLM types
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

// ############### Engine Wide Macros/Defines ###############

#define PXE_NODISCARD [[nodiscard]]
#define PXE_NOCOPY(className) 		className(const className& other) = delete;\
									className operator=(const className& other) = delete

namespace pxengine {
	// ############### PxeEngine Types ###############

	typedef std::size_t PxeSize;

	enum class PxeLogLevel : uint8_t
	{
		PXE_INFO = 0,
		PXE_WARN,
		PXE_ERROR,
		PXE_FATAL,
	};

	// ############### PxeGlAsset Types ###############

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

	// ############### PxeIndexBuffer Types ###############

	enum class PxeIndexType : uint32_t
	{
		UNDEFINED = 0,
		UNSIGNED_8BIT = 0x1401,  // GL_UNSIGNED_BYTE
		UNSIGNED_16BIT = 0x1403, // GL_UNSIGNED_SHORT
		UNSIGNED_32BIT = 0x1405, // GL_UNSIGNED_INT
	};

	// ############### PxeShader Types ###############

	// PxeShaderId is unique to each PxeShader type
	// Note: this is not technically unique to each PxeShader instance, but there should only be one instance at a time
	typedef uint32_t PxeShaderId;

	// Shader will be executed in ascending order
	typedef uint16_t PxeShaderExecutionOrder;

	namespace PxeDefaultShaderExecutionOrder {

		enum PxeDefaultShaderExecutionOrderEnum : PxeShaderExecutionOrder
		{
			PRE_FRAME = 0,		// Note: will be executed before framebuffers are created, primarily used for updating PxeRenderTextures
			PRE_RENDER = 0x2000,
			OPAQUE = 0x4000,
			TRANSPARENT = 0x6000,
			POST_PROCESSING = 0x8000,
			OVERLAY = 0xA000,
			GUI = 0xC000,
			POST_RENDER = 0xE000,
		};
	}

	// ############### PxeComponent Types ###############

	// PxeComponentId is unique to each PxeComponent type
	// Note: this is not unique to each instance of a PxeComponent
	typedef uint32_t PxeComponentId;

	// ############### PxeScene Types ###############

	typedef uint8_t PxeSceneFlags;
	namespace PxeSceneFlag {
		enum PxeSceneFlagsEnum : PxeSceneFlags
		{
			NONE = 0x00,
			PHYSICS_UPDATE = 0x01,
			COMPONENT_UPDATE = 0x02,
			ALL = 0x03
		};
	}

	// ############### PxeVertexBuffer Types ###############

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

	// ############### PxeFont Types ###############

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

	// ############### Input System Types ###############

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

	typedef int32_t SDL_Keycode;

	// Creates a PxeActionSourceCode from PxeActionSourceType and the desired actionCode
	PXE_NODISCARD inline constexpr PxeActionSourceCode CreatePxeActionSourceCode(PxeActionSourceType sourceType, int32_t actionCode) { return (PxeActionSourceCode)sourceType << 32 | actionCode; }

	// Creates a PxeActionSourceCode from a SDL_Keycode
	PXE_NODISCARD inline constexpr PxeActionSourceCode PxeKeyboardActionSourceCode(SDL_Keycode keyboardKey) { return CreatePxeActionSourceCode(PxeActionSourceType::KEYBOARD, keyboardKey); }

	// Creates a PxeActionSourceCode from a SDL mouse button index
	PXE_NODISCARD inline constexpr PxeActionSourceCode PxeMouseActionSourceCode(uint8_t mouseButton) { return CreatePxeActionSourceCode(PxeActionSourceType::MOUSE, static_cast<int32_t>(mouseButton)); }

	// Creates a PxeActionSourceCode from a SDL joystick button index
	PXE_NODISCARD inline constexpr PxeActionSourceCode PxeJoystickButtonActionSourceCode(uint8_t joystickButton) { return CreatePxeActionSourceCode(PxeActionSourceType::JOYSTICK_BUTTON, static_cast<int32_t>(joystickButton)); }

	// Creates a PxeActionSourceCode from a SDL joystick hat index and hat direction
	PXE_NODISCARD inline constexpr PxeActionSourceCode PxeJoystickHatActionSourceCode(uint8_t hatIndex, uint8_t hatDir) { return CreatePxeActionSourceCode(PxeActionSourceType::JOYSTICK_HAT, (static_cast<int32_t>(hatIndex) << 8) | hatDir); }

	// Creates a PxeActionSourceCode from a SDL controller button index
	PXE_NODISCARD inline constexpr PxeActionSourceCode PxeControllerButtonActionSourceCode(uint8_t controllerButton) { return CreatePxeActionSourceCode(PxeActionSourceType::CONTROLLER_BUTTON, static_cast<int32_t>(controllerButton)); }
}
#endif // !PXENGINE_TYPES_H_
