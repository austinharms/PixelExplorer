#include "common/RefCount.h"
#include "glm/vec3.hpp"
#include "PlayerCamera.h"
#include "engine/input/InputManager.h"

#ifndef PIXELEXPLOERE_GAME_PLAYER_PLAYER_H_
#define PIXELEXPLOERE_GAME_PLAYER_PLAYER_H_
namespace pixelexplorer::game::player {
	class Player : public RefCount
	{
	public:
		// TODO add description
		// note this will not set it's own camera to be the active camera
		Player(const glm::vec3& position, uint32_t dimensionId, engine::input::InputManager& inputManager);
		virtual ~Player();

		// update the player inputs and camera using deltaTime
		void update(double deltaTime);

		// returns the id of the dimension the player is in
		uint32_t getDimensionId() const;

		const glm::vec3& getPosition() const;

		void setPosition(const glm::vec3& position);

		PlayerCamera& getCamera() const;

	private:
		glm::vec3 _position;
		PlayerCamera* _camera;
		uint32_t _dimensionId;
	};
}
#endif // !PIXELEXPLOERE_GAME_PLAYER_PLAYER_H_
