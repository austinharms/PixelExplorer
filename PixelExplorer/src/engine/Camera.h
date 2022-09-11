#include "rendering/CameraInterface.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/glm/ext/matrix_transform.hpp"
#include "glm/glm/ext/matrix_clip_space.hpp"
#include "glm/glm/gtx/euler_angles.hpp"

#ifndef PIXELEXPLORER_ENGINE_CAMERA_H_
#define PIXELEXPLORER_ENGINE_CAMERA_H_
namespace pixelexplorer::engine {
	class Camera : public rendering::CameraInterface
	{
	public:
		inline Camera() {
			_position = glm::vec3(0, 0, -5);
			_rotationRad = glm::vec3(0, glm::pi<float>(), glm::half_pi<float>());
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
			_forward = computeForward();
			_up = computeUp();
			_right = glm::cross(_forward, _up);
			_vpMatrix = glm::perspective(_fov, _aspectRatio, 0.1f, 100.0f) * glm::lookAt(_position, _position + _forward, _up);
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

		// set the camera rotation in radians
		inline void setRotation(const glm::vec3& rotation) {
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

		// returns the camera rotation in radians 
		inline const glm::vec3& getRotation() const {
			return _rotationRad;
		}

		inline const glm::vec3& getForward() const {
			return _forward;
		}

		inline const glm::vec3& getRight() const {
			return _right;
		}

		inline const glm::vec3& getUp() const {
			_up;
		}

	private:
		glm::mat4 _vpMatrix;
		glm::vec3 _position;
		glm::vec3 _rotationRad;
		glm::vec3 _forward;
		glm::vec3 _up;
		glm::vec3 _right;
		float _aspectRatio;
		float _fov;
		bool _active;

		inline glm::vec3 computeForward() {
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

		inline glm::vec3 computeUp() {
			return glm::vec3(cosf(_rotationRad.z), sinf(_rotationRad.z), 0);
		}
	};
}
#endif // !PIXELEXPLORER_ENGINE_CAMERA_H_
