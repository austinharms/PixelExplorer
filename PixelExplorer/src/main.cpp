#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "Log.h"
#include "Application.h"
#include "PxeEngine.h"

int main(void) {
	pixelexplorer::Log log;
	log.onLog(pxengine::PxeLogLevel::PXE_INFO, "PixelExplore start", __FILE__, __FUNCTION__, __LINE__);
	pixelexplorer::Application::getInstance().start(log);
	log.onLog(pxengine::PxeLogLevel::PXE_INFO, "PixelExplore stop", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}