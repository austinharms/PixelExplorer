#include "Log.h"
#include "Application.h"
#include "PxeEngine.h"

int main(void) {
	pxengine::pxeRunApplication(pixelexplorer::Application::getInstance(), pixelexplorer::Log::getInstance());
	return 0;
}