#include "Camera.h"

#include "PxeEngine.h"
#include "PxeInputManager.h"
#include "PxeActionSource.h"
#include "SDL.h"

namespace pixelexplorer {
	Camera::Camera(pxengine::PxeWindow& targetWindow, float fov, float nearClip, float farClip) {
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

		pxengine::PxeInputManager& inputMgr = pxengine::pxeGetEngine().getInputManager();
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
			}
		}

		targetWindow.setCamera(this);
		// Don't grab here as the window will grab the camera and grabbing the window would create a circular chain
		// also we don't use _targetWindow as a reference but to check if we should be using the inputs
		_targetWindow = &targetWindow;
		lockCursor();
	}

	Camera::~Camera() {
		unlockCursor();
	}

	PXE_NODISCARD glm::mat4 Camera::getPVMatrix() const {
		return _pvMatrix;
	}

	void Camera::setWindowSize(int32_t width, int32_t height) {
		if (_width != width || _height != height) {
			_width = width;
			_height = height;
			updateProjectionMatrix();
		}
	}

	void Camera::update() {
		if (_error) return;
		pxengine::PxeInputManager& inputMgr = pxengine::pxeGetEngine().getInputManager();
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

		float dt = pxengine::pxeGetEngine().getDeltaTime();
		for (uint8_t i = 0; i < ACTION_COUNT; ++i) {
			float value = _actions[i]->getValue();
			if (!value) continue;
			if (i % 2 == 1) value = -value;
			_position += _directionVectors[i / 2] * value * dt * 7.5f;
		}

		updatePVMatrix();
	}

	PXE_NODISCARD bool Camera::getErrorFlag() const { return _error; }

	PXE_NODISCARD float Camera::getFOV() const { return _fov; }

	PXE_NODISCARD float Camera::getNearClip() const { return _near; }

	PXE_NODISCARD float Camera::getFarClip() const { return _far; }

	PXE_NODISCARD const glm::vec3& Camera::getPosition() const { return _position; }

	PXE_NODISCARD const glm::vec3& Camera::getForward() const { return _directionVectors[2]; }

	PXE_NODISCARD const glm::vec3& Camera::getRotation() const { return _rotation; }

	void Camera::setFOV(float value) {
		_fov = value;
		updateProjectionMatrix();
	}

	void Camera::setNearClip(float value) {
		_near = value;
		updateProjectionMatrix();
	}

	void Camera::setFarClip(float value) {
		_far = value;
		updateProjectionMatrix();
	}

	void Camera::setPosition(const glm::vec3& pos) {
		_position = pos;
		updatePVMatrix();
	}

	void Camera::setRotation(const glm::vec3& rot) {
		_rotation = rot;
		updateDirectionVectors();
		updatePVMatrix();
	}

	void Camera::lockCursor() {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	void Camera::unlockCursor() {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void Camera::updateProjectionMatrix() {
		if (!_width || !_height) return;
		_projectionMatrix = glm::perspective(_fov, (float)_width / (float)_height, _near, _far);
		updatePVMatrix();
	}

	void Camera::updatePVMatrix() {
		_pvMatrix = _projectionMatrix * glm::lookAt(_position, _position + _directionVectors[2], _directionVectors[0]);
	}

	void Camera::updateDirectionVectors() {
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
}
