#include <stdint.h>

#include "CameraInterface.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "glm/glm/ext/matrix_clip_space.hpp"

#ifndef PIXELEXPLORER_RENDERING_CAMERA_H_
#define PIXELEXPLORER_RENDERING_CAMERA_H_
namespace pixelexplorer::rendering {
	class Camera : public CameraInterface
	{
	public:
		inline Camera() {
			_position = glm::vec3(0, 0, -5);
			_forward = glm::vec3(0, 0, 1);
			_aspectRatio = 1;
			_fov = 90;
			_active = false;
			updateVPMatrix();
		}

		inline virtual ~Camera() {}

		inline const glm::mat4& getVPMatrix() const override {
			return _vpMatrix;
		}

		inline void windowResize(uint32_t width, uint32_t height) override {
			_aspectRatio = (float)width / (float)height;
			updateVPMatrix();
		}

		inline void setActive(bool active) override {
			_active = active;
		}

		inline void updateVPMatrix() {
			_vpMatrix = glm::perspective(_fov, _aspectRatio, 0.1f, 100.0f) * glm::lookAt(_position, _position + _forward, glm::vec3(0, 1, 0));
		}

		inline void setFOV(float fov) {
			_fov = fov;
			updateVPMatrix();
		}

		inline float getFOV() const {
			return _fov;
		}

		inline void setPosition(const glm::vec3& pos) {
			_position = pos;
			updateVPMatrix();
		}

		inline const glm::vec3& getPosition() const {
			return _position;
		}

		inline void setForward(const glm::vec3& forward) {
			_forward = forward;
			updateVPMatrix();
		}

		inline const glm::vec3& getForward() const {
			return _forward;
		}

	private:
		glm::mat4 _vpMatrix;
		glm::vec3 _position;
		glm::vec3 _forward;
		float _aspectRatio;
		float _fov;
		bool _active;
	};
}
#endif // !PIXELEXPLORER_RENDERING_CAMERA_H_
