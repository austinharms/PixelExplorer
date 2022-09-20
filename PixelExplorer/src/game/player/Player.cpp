#include "Player.h"

#include "common/Logger.h"

namespace pixelexplorer::game::player {
	Player::Player(const glm::vec3& position, engine::input::InputManager& inputManager)
	{
		_camera = new(std::nothrow) PlayerCamera(inputManager);
		if (_camera == nullptr) Logger::fatal(__FUNCTION__" failed to allocate player camera");
		_position = position;
	}

	Player::~Player()
	{
		_camera->drop();
	}

	void Player::update(double deltaTime)
	{
		_camera->update(deltaTime);
	}
}