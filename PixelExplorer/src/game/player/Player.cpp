#include "Player.h"

#include "common/Logger.h"

namespace pixelexplorer::game::player {
	Player::Player(const glm::vec3& position, uint32_t dimensionId, engine::input::InputManager& inputManager)
	{
		_camera = new(std::nothrow) PlayerCamera(inputManager);
		if (_camera == nullptr) Logger::fatal(__FUNCTION__" failed to allocate player camera");
		_position = position;
		_dimensionId = dimensionId;
	}

	Player::~Player()
	{
		_camera->drop();
	}

	void Player::update(double deltaTime)
	{
		_camera->update(deltaTime);
	}

	uint32_t Player::getDimensionId() const
	{
		return _dimensionId;
	}

	const glm::vec3& Player::getPosition() const
	{
		return _position;
	}

	void Player::setPosition(const glm::vec3& position)
	{
		_position = position;
	}
	PlayerCamera& Player::getCamera() const
	{
		return *_camera;
	}
}