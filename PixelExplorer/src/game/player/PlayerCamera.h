#include "engine/input/InputAction.h"
#include "engine/input/InputManager.h"
#include "../engine/rendering/CameraInterface.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#ifndef PIXELEXPLORER_GAME_PLAYER_PLAYERCAMERA_H_
#define PIXELEXPLORER_GAME_PLAYER_PLAYERCAMERA_H_
namespace pixelexplorer::game::player {
	class PlayerCamera : public engine::rendering::CameraInterface
	{
	public:
		// a simple first person camera that uses InputAction's to manipulate the camera
		PlayerCamera(engine::input::InputManager& inputManager);
		virtual ~PlayerCamera();

		// returns the combined projection and view matrix
		const glm::mat4& getVPMatrix() const override;

		// used to update the camera projection matrix on window resizing
		void windowResize(uint32_t width, uint32_t height) override;

		// used to track if this camera is being used
		void setActive(bool active) override;

		// set the FOV of the camera
		void setFOV(float fov);

		// returns the FOV of the camera
		float getFOV() const;

		// set the camera position
		void setPosition(const glm::vec3& pos);

		// returns the camera position
		const glm::vec3& getPosition() const;

		// set the camera rotation in radians
		void setRotation(const glm::vec3& rotation);

		// returns the camera rotation in radians 
		const glm::vec3& getRotation() const;

		// returns the forward vector of the camera
		const glm::vec3& getForward() const;

		// returns the right vector of the camera
		const glm::vec3& getRight() const;

		// returns the up vector of the camera
		const glm::vec3& getUp() const;

		// update the camera input actions using deltaTime
		// without calling this the camera will not move when inputs are triggered
		void update(double deltaTime);

	private:
		engine::input::InputAction* _inputActions[10];
		glm::mat4 _vpMatrix;
		glm::vec3 _position;
		glm::vec3 _rotationRad;
		glm::vec3 _forward;
		glm::vec3 _up;
		glm::vec3 _right;
		float _aspectRatio;
		float _fov;
		bool _active;

		glm::vec3 computeForward();
		glm::vec3 computeUp();
		void dropActions();
		void registerActions(engine::input::InputManager& inputManager);
		void updateVPMatrix();
	};
}
#endif // !PIXELEXPLORER_GAME_FPSCAMERA_H_
