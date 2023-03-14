#ifndef PXENGINESAMPELS_SCENES_FREE_CAMERA_CAMERA_H_
#define PXENGINESAMPELS_SCENES_FREE_CAMERA_CAMERA_H_
#include "PxeTypes.h"
#include "PxeCamera.h"
#include "PxeEngine.h"
#include "PxeInputManager.h"
#include "PxeAction.h"
#include "PxeActionSource.h"
#include "PxeWindow.h"
#include "SDL_keycode.h"
#include <SDL_mouse.h>

class Camera : public pxengine::PxeCamera
{
public:
	Camera(pxengine::PxeWindow& targetWindow, float fov, float nearClip = 0.1f, float farClip = 100.0f) {
		_targetWindow = nullptr;
		_width = -1;
		_height = -1;
		_error = false;
		_fov = fov;
		_near = nearClip;
		_far = farClip;
		_position = glm::vec3(0);
		_rotation = glm::vec3(0, glm::pi<float>(), glm::half_pi<float>());
		updateDirectionVectors();

		pxengine::PxeInputManager& inputMgr = pxengine::PxeEngine::getInstance().getInputManager();
		for (uint8_t i = 0; i < ACTION_COUNT; ++i) {
			_actions[i] = inputMgr.getAction(ACTION_NAMES[i]);
			if (!_actions[i]) {
				_error = true;
				return;
			}

			if (!_actions[i]->hasSource()) {
				pxengine::PxeActionSource* src = inputMgr.getActionSource(DEFAULT_ACTION_CODES[i]);
				if (!src) {
					_error = true;
					return;
				}

				_actions[i]->addSource(*src);
				src->drop();
			}
		}

		targetWindow.setCamera(this);
		// Don't grab here as the window will grab the camera and grabbing the window would create a circular chain
		// also we don't use _targetWindow as a reference but to check if we should be using the inputs
		_targetWindow = &targetWindow;
		lockCursor();
	}

	virtual ~Camera() {
		unlockCursor();
		for (uint8_t i = 0; i < ACTION_COUNT; ++i) {
			if (_actions[i])
				_actions[i]->drop();
		}
	}

	PXE_NODISCARD glm::mat4 getPVMatrix() const override {
		return _pvMatrix;
	}

	void setWindowSize(int32_t width, int32_t height) override {
		if (_width != width || _height != height) {
			_width = width;
			_height = height;
			updateProjectionMatrix();
		}
	}

	void update() {
		if (_error) return;
		pxengine::PxeInputManager& inputMgr = pxengine::PxeEngine::getInstance().getInputManager();
		if (inputMgr.getKeyboardFocusedWindow() != _targetWindow) return;
		const glm::i32vec2& cursorChange = inputMgr.getCursorChange();
		_rotation.y += glm::radians((float)cursorChange.x);
		_rotation.x += glm::radians((float)cursorChange.y);
		constexpr float lookMax = glm::half_pi<float>() - 0.001f;
		if (_rotation.x < -lookMax) {
			_rotation.x = -lookMax;
		}
		else if (_rotation.x > lookMax) {
			_rotation.x = lookMax;
		}
		updateDirectionVectors();

		float dt = pxengine::PxeEngine::getInstance().getDeltaTime();
		for (uint8_t i = 0; i < ACTION_COUNT; ++i) {
			float value = _actions[i]->getValue();
			if (!value) continue;
			if (i % 2 == 1) value = -value;
			_position += _directionVectors[i / 2] * value * dt * 2.0f;
		}

		updatePVMatrix();
	}

	PXE_NODISCARD bool getErrorFlag() const { return _error; }

	PXE_NODISCARD float getFOV() const { return _fov; }

	PXE_NODISCARD float getNearClip() const { return _near; }

	PXE_NODISCARD float getFarClip() const { return _far; }

	PXE_NODISCARD glm::vec3 getPosition() const override { return _position; }

	PXE_NODISCARD const glm::vec3& getRotation() const { return _rotation; }

	void setFOV(float value) {
		_fov = value;
		updateProjectionMatrix();
	}

	void setNearClip(float value) {
		_near = value;
		updateProjectionMatrix();
	}

	void setFarClip(float value) {
		_far = value;
		updateProjectionMatrix();
	}

	void setPosition(const glm::vec3& pos) {
		_position = pos;
		updatePVMatrix();
	}

	void setRotation(const glm::vec3& rot) {
		_rotation = rot;
		updateDirectionVectors();
		updatePVMatrix();
	}

	void lockCursor() {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	void unlockCursor() {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

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

	const pxengine::PxeActionSourceCode DEFAULT_ACTION_CODES[ACTION_COUNT] = {
		pxengine::PxeKeyboardActionSourceCode(SDLK_SPACE),
		pxengine::PxeKeyboardActionSourceCode(SDLK_c),
		pxengine::PxeKeyboardActionSourceCode(SDLK_d),
		pxengine::PxeKeyboardActionSourceCode(SDLK_a),
		pxengine::PxeKeyboardActionSourceCode(SDLK_w),
		pxengine::PxeKeyboardActionSourceCode(SDLK_s),
	};

	const char* ACTION_NAMES[ACTION_COUNT] = {
		"Camera Up",
		"Camera Down",
		"Camera Right",
		"Camera Left",
		"Camera Forward",
		"Camera Backward"
	};

	void updateProjectionMatrix() {
		if (!_width || !_height) return;
		_projectionMatrix = glm::perspective(_fov, (float)_width / (float)_height, _near, _far);
		updatePVMatrix();
	}

	void updatePVMatrix() {
		_pvMatrix = _projectionMatrix * glm::lookAt(_position, _position + _directionVectors[2], _directionVectors[0]);
	}

	void updateDirectionVectors() {
		// Up Vector
		_directionVectors[0] = glm::vec3(cosf(_rotation.z), sinf(_rotation.z), 0);

		// Forward Vector
		float cosY = cosf(_rotation.y * 0.5f);
		float sinY = sinf(_rotation.y * 0.5f);
		float cosX = cosf(_rotation.x * 0.5f);
		float sinX = sinf(_rotation.x * 0.5f);
		float x = cosX * cosY;
		float y = -sinX * sinY;
		float z = cosX * sinY;
		float w = sinX * cosY;
		_directionVectors[2] = glm::vec3(2 * (x * z + w * y), 2 * (y * z - w * x), 1 - 2 * (x * x + y * y));

		// Left Vector
		_directionVectors[1] = glm::cross(_directionVectors[2], _directionVectors[0]);
	}

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
#endif // !PXENGINESAMPLES_FREE_CAMERA_CAMERA_H_
