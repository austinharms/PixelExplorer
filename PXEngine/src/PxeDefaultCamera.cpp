#include "PxeDefaultCamera.h"

#include <new>

#include "NpLogger.h"

namespace pxengine {
	PxeDefaultCamera* PxeDefaultCamera::createCamera(float fov, float near, float far) {
		PxeDefaultCamera* cam = new(std::nothrow) PxeDefaultCamera();
		if (!cam) {
			PXE_ERROR("Failed to allocate default camera");
			return nullptr;
		}

		cam->setProjectionPerspective(fov, near, far);
		return cam;
	}

	PxeDefaultCamera* PxeDefaultCamera::createCamera(float near, float far) {
		PxeDefaultCamera* cam = new(std::nothrow) PxeDefaultCamera();
		if (!cam) {
			PXE_ERROR("Failed to allocate default camera");
			return nullptr;
		}

		cam->setProjectionOrthographic(near, far);
	}

	PxeDefaultCamera::PxeDefaultCamera() {
		lookAt(glm::vec3(0, 0, -10), glm::vec3(0), glm::vec3(0, 1, 0));
		_width = -1;
		_height = -1;
	}

	void PxeDefaultCamera::setProjectionOrthographic(float near, float far)
	{
		_perspectiveProjection = false;
		_projectionProperties[0] = near;
		_projectionProperties[1] = far;
		_width = -1;
		_height = -1;
	}

	void PxeDefaultCamera::setProjectionPerspective(float fov, float near, float far)
	{
		_perspectiveProjection = true;
		_projectionProperties[0] = fov;
		_projectionProperties[1] = near;
		_projectionProperties[2] = far;
		_width = -1;
		_height = -1;
	}

	void PxeDefaultCamera::lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3 up)
	{
		_viewMatrix = glm::lookAt(position, target, up);
	}

	void PxeDefaultCamera::translate(const glm::vec3& direction)
	{
		_viewMatrix = glm::translate(_viewMatrix, direction);
	}

	void PxeDefaultCamera::rotateAbout(const glm::vec3& axis, float rad)
	{
		_viewMatrix = glm::rotate(_viewMatrix, rad, axis);
	}

	PXE_NODISCARD glm::mat4 PxeDefaultCamera::getPVMatrix() const
	{
		return _projectionMatrix * _viewMatrix;
	}

	void PxeDefaultCamera::setWindowSize(int32_t width, int32_t height)
	{
		if (width == _width && height == _height) return;
		_width = width;
		_height = height;
		if (_perspectiveProjection) {
			_projectionMatrix = glm::perspective(_projectionProperties[0], (float)_width / _height, _projectionProperties[1], _projectionProperties[2]);
		}
		else {
			_projectionMatrix = glm::ortho(-(float)_width / 2, (float)_width / 2, -(float)_height / 2, (float)_height / 2, _projectionProperties[0], _projectionProperties[1]);
		}
	}
}