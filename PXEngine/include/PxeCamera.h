#ifndef PXENGINE_CAMERA_H_
#define PXENGINE_CAMERA_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	class PxeCamera : public PxeRefCount
	{
	public:
		PxeCamera() = default;
		virtual ~PxeCamera() = default;

		// Should return the projection and view matrix multiplied together
		virtual PXE_NODISCARD glm::mat4 getPVMatrix() const = 0;

		// Called by the PxeWindow every frame with the windows size in pixels
		virtual void setWindowSize(int32_t width, int32_t height) = 0;
	};
}
#endif // !PXENGINE_CAMAERA_H_
