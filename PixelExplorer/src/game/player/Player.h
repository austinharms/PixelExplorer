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
		Player(const glm::vec3& position, engine::input::InputManager& inputManager);
		virtual ~Player();

		// update the player inputs and camera using deltaTime
		void update(double deltaTime);

	private:
		glm::vec3 _position;
		PlayerCamera* _camera;
	};
}
#endif // !PIXELEXPLOERE_GAME_PLAYER_PLAYER_H_
