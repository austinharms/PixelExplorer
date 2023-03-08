#ifndef PXENGINE_DEFAULT_CAMERA_H_
#define PXENGINE_DEFAULT_CAMERA_H_
#include "PxeTypes.h"
#include "PxeCamera.h"

namespace pxengine {
	class PxeDefaultCamera : public PxeCamera
	{
	public:
		// Returns a new PxeDefaultCamera instance with a perspective projection
		static PXE_NODISCARD PxeDefaultCamera* createCamera(float fov, float near, float far);

		// Returns a new PxeDefaultCamera instance with a orthographic projection
		static PXE_NODISCARD PxeDefaultCamera* createCamera(float near, float far);

		// Sets the projection matrix to an orthographic projection, with {near} and {far} clipping panes
		void setProjectionOrthographic(float near, float far);

		// Sets the projection matrix to an perspective projection, with a field of view of {fov}rad with {near} and {far} clipping panes
		void setProjectionPerspective(float fov, float near, float far);

		// Sets the camera to be at {position} and looking at {target} with an up vector of {up}
		void lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3 up);

		// Moves the camera by {direction}
		void translate(const glm::vec3& direction);

		// Rotate the camera on {axis} be {rad} radians
		void rotateAbout(const glm::vec3& axis, float rad);

		PXE_NODISCARD glm::vec3 getPosition() const override;
		PXE_NODISCARD glm::mat4 getPVMatrix() const override;
		void setWindowSize(int32_t width, int32_t height) override;
		virtual ~PxeDefaultCamera() = default;

	private:
		PxeDefaultCamera();

		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;
		float _projectionProperties[3];
		int32_t _width;
		int32_t _height;
		bool _perspectiveProjection;
	};
}

#endif // !PXENGINE_DEFAULT_CAMERA_H_
