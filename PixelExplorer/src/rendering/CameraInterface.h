#include <stdint.h>

#include "RefCount.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORER_RENDERING_CAMERAINTERFACE_H_
#define PIXELEXPLORER_RENDERING_CAMERAINTERFACE_H_
namespace pixelexplorer::rendering {
	class CameraInterface : public RefCount
	{
	public:
		inline virtual ~CameraInterface() {}
		virtual const glm::mat4& getVPMatrix() const = 0;
		virtual void windowResize(uint32_t width, uint32_t height) = 0;
		virtual void setActive(bool active) = 0;
	};
}
#endif // !PIXELEXPLORER_RENDERING_CAMERA_H_
