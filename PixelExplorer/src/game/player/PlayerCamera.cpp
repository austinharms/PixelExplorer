#include "PlayerCamera.h"

#include <stdint.h>

#include "engine/input/InputSource.h"
#include "engine/input/InputAction.h"
#include "engine/input/InputManager.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "glm/glm/ext/matrix_clip_space.hpp"

namespace pixelexplorer::game::player {
	PlayerCamera::PlayerCamera(engine::input::InputManager& inputManager) {
		using namespace engine::input;
		_position = glm::vec3(0, 0, -5);
		_rotationRad = glm::vec3(0, glm::pi<float>(), glm::half_pi<float>());
		_aspectRatio = 1;
		_fov = 90;
		_active = false;
		updateVPMatrix();
		memset(_inputActions, 0, sizeof(_inputActions));
		registerActions(inputManager);
	}

	PlayerCamera::~PlayerCamera() { dropActions(); }

	const glm::mat4& PlayerCamera::getVPMatrix() const {
		return _vpMatrix;
	}

	void PlayerCamera::windowResize(uint32_t width, uint32_t height) {
		_aspectRatio = (float)width / (float)height;
		updateVPMatrix();
	}

	void PlayerCamera::setActive(bool active) {
		_active = active;
	}

	void PlayerCamera::updateVPMatrix() {
		_forward = computeForward();
		_up = computeUp();
		_right = glm::cross(_forward, _up);
		_vpMatrix = glm::perspective(_fov, _aspectRatio, 0.1f, 1000.0f) * glm::lookAt(_position, _position + _forward, _up);
	}

	void PlayerCamera::setFOV(float fov) {
		_fov = fov;
		updateVPMatrix();
	}

	float PlayerCamera::getFOV() const {
		return _fov;
	}

	void PlayerCamera::setPosition(const glm::vec3& pos) {
		_position = pos;
		updateVPMatrix();
	}

	const glm::vec3& PlayerCamera::getPosition() const {
		return _position;
	}

	void PlayerCamera::setRotation(const glm::vec3& rotation) {
		_rotationRad = rotation;
		constexpr float lookMax = glm::half_pi<float>() - 0.001f;
		if (_rotationRad.x < -lookMax) {
			_rotationRad.x = -lookMax;
		}
		else if (_rotationRad.x > lookMax) {
			_rotationRad.x = lookMax;
		}

		updateVPMatrix();
	}

	const glm::vec3& PlayerCamera::getRotation() const {
		return _rotationRad;
	}

	const glm::vec3& PlayerCamera::getForward() const {
		return _forward;
	}

	const glm::vec3& PlayerCamera::getRight() const {
		return _right;
	}

	const glm::vec3& PlayerCamera::getUp() const {
		return _up;
	}

	void PlayerCamera::update(double deltaTime) {
		glm::vec3 lookRotation(
			_inputActions[0]->getTrueValue(deltaTime) - _inputActions[1]->getTrueValue(deltaTime),
			_inputActions[2]->getTrueValue(deltaTime) - _inputActions[3]->getTrueValue(deltaTime),
			_inputActions[5]->getTrueValue(deltaTime) * 90.0 - _inputActions[4]->getTrueValue(deltaTime) * 90.0);
		if (lookRotation != glm::vec3(0)) setRotation(_rotationRad + glm::radians(lookRotation));
		glm::vec3 movePosition(
			(_forward * (float)(_inputActions[6]->getTrueValue(deltaTime) - _inputActions[7]->getTrueValue(deltaTime)) * 100.0f) +
			(_right * (float)(_inputActions[9]->getTrueValue(deltaTime) - _inputActions[8]->getTrueValue(deltaTime)) * 100.0f));
		if (movePosition != glm::vec3(0)) setPosition(_position + movePosition);
	}

	glm::vec3 PlayerCamera::computeForward() {
		float cosY = cosf(_rotationRad.y * 0.5f);
		float sinY = sinf(_rotationRad.y * 0.5f);
		float cosX = cosf(_rotationRad.x * 0.5f);
		float sinX = sinf(_rotationRad.x * 0.5f);

		float x = cosX * cosY;
		float y = -sinX * sinY;
		float z = cosX * sinY;
		float w = sinX * cosY;
		return glm::vec3(2 * (x * z + w * y), 2 * (y * z - w * x), 1 - 2 * (x * x + y * y));
	}

	glm::vec3 PlayerCamera::computeUp() {
		return glm::vec3(cosf(_rotationRad.z), sinf(_rotationRad.z), 0);
	}

	void PlayerCamera::dropActions() {
		for (uint32_t i = 0; i < 10; ++i) {
			if (_inputActions[i]) _inputActions[i]->drop();
			_inputActions[i] = nullptr;
		}
	}

	void PlayerCamera::registerActions(engine::input::InputManager& inputManager) {
		using namespace engine::input;
		inputManager.grab();
		dropActions();
		_inputActions[0] = inputManager.getOrCreateAction("look up", InputSource{ InputSource::InputSourceType::CURSORPOS, PX_Y_AXIS, false, false });
		_inputActions[1] = inputManager.getOrCreateAction("look down", InputSource{ InputSource::InputSourceType::CURSORPOS, PX_Y_AXIS, true, false });
		_inputActions[2] = inputManager.getOrCreateAction("look left", InputSource{ InputSource::InputSourceType::CURSORPOS, PX_X_AXIS, false, false });
		_inputActions[3] = inputManager.getOrCreateAction("look right", InputSource{ InputSource::InputSourceType::CURSORPOS, PX_X_AXIS, true, false });
		_inputActions[4] = inputManager.getOrCreateAction("look counterclockwise", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_Q) });
		_inputActions[5] = inputManager.getOrCreateAction("look clockwise", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_E) });
		_inputActions[6] = inputManager.getOrCreateAction("move forward", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_W) });
		_inputActions[7] = inputManager.getOrCreateAction("move backward", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_S) });
		_inputActions[8] = inputManager.getOrCreateAction("move left", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_A) });
		_inputActions[9] = inputManager.getOrCreateAction("move right", InputSource{ InputSource::InputSourceType::KEYBOARD, glfwGetKeyScancode(GLFW_KEY_D) });
		for (uint32_t i = 0; i < 10; ++i)	_inputActions[i]->grab();
		inputManager.drop();
	}
}
