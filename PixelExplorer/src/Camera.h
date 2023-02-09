#ifndef PIXELEXPLORER_CAMERA_H_
#define PIXELEXPLORER_CAMERA_H_
#include "PxeTypes.h"
#include "PxeCamera.h"
#include "PxeAction.h"
#include "PxeWindow.h"
#include "SDL_keycode.h"

namespace pixelexplorer {
	class Camera : public pxengine::PxeCamera
	{
	public:
		Camera(pxengine::PxeWindow& targetWindow, float fov, float nearClip = 0.1f, float farClip = 100.0f);
		virtual ~Camera();
		PXE_NODISCARD glm::mat4 getPVMatrix() const override;
		void setWindowSize(int32_t width, int32_t height) override;
		void update();
		PXE_NODISCARD bool getErrorFlag() const;
		PXE_NODISCARD float getFOV() const;
		PXE_NODISCARD float getNearClip() const;
		PXE_NODISCARD float getFarClip() const;
		PXE_NODISCARD const glm::vec3& getForward() const;
		PXE_NODISCARD const glm::vec3& getPosition() const;
		PXE_NODISCARD const glm::vec3& getRotation() const;
		void setFOV(float value);
		void setNearClip(float value);
		void setFarClip(float value);
		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::vec3& rot);
		void lockCursor();
		void unlockCursor();

	private:
		enum InputActions
		{
			MOVE_UP = 0,
			MOVE_DOWN,
			MOVE_RIGHT,
			MOVE_LEFT,
			MOVE_FORWARD,
			MOVE_BACKWARD,
			ACTION_COUNT
		};

		static constexpr pxengine::PxeActionSourceCode DEFAULT_ACTION_CODES[ACTION_COUNT] = {
			(uint64_t)pxengine::PxeActionSourceType::KEYBOARD << 32 | SDLK_SPACE,
			(uint64_t)pxengine::PxeActionSourceType::KEYBOARD << 32 | SDLK_c,
			(uint64_t)pxengine::PxeActionSourceType::KEYBOARD << 32 | SDLK_d,
			(uint64_t)pxengine::PxeActionSourceType::KEYBOARD << 32 | SDLK_a,
			(uint64_t)pxengine::PxeActionSourceType::KEYBOARD << 32 | SDLK_w,
			(uint64_t)pxengine::PxeActionSourceType::KEYBOARD << 32 | SDLK_s,
		};

		const char* ACTION_NAMES[ACTION_COUNT] = {
			"Camera Up",
			"Camera Down",
			"Camera Right",
			"Camera Left",
			"Camera Forward",
			"Camera Backward"
		};

		void updateProjectionMatrix();
		void updatePVMatrix();
		void updateDirectionVectors();

		pxengine::PxeWindow* _targetWindow;
		pxengine::PxeAction* _actions[ACTION_COUNT];
		glm::mat4 _projectionMatrix;
		glm::mat4 _pvMatrix;
		glm::vec3 _position;
		glm::vec3 _rotation;
		glm::vec3 _directionVectors[3];
		int32_t _width;
		int32_t _height;
		float _fov;
		float _near;
		float _far;
		bool _error;
	};
}
#endif // !PIXELEXPLORER_CAMERA_H_
