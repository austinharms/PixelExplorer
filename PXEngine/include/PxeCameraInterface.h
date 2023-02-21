#ifndef PXENGINE_CAMERA_INTERFACE_H_
#define PXENGINE_CAMERA_INTERFACE_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeCameraInterface : public PxeRefCount
	{
	public:
		PxeCameraInterface() = default;
		virtual ~PxeCameraInterface() = default;

		// Should return the projection and view matrix multiplied together
		PXE_NODISCARD virtual glm::mat4 getPVMatrix() const = 0;

		// Called by the PxeRenderPipeline before every scene render
		// Note: the width and height are in pixels
		virtual void setWindowSize(int32_t width, int32_t height) = 0;

		// Should return the cameras position in world space
		PXE_NODISCARD virtual glm::vec3 getPosition() const = 0;
	};
}
#endif // !PXENGINE_CAMERA_INTERFACE_H_
