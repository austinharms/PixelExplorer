#include <iostream>

#include "PxeEngine.h"
#include "PxeWindow.h"
#include "SDL.h"
#include "GL/glew.h"


class LogHandle : public pxengine::PxeLogHandler {
public:
	virtual ~LogHandle() {}
	void onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function) override {
		std::cout << "Log: " << msg << " Level: " << level << " File: " << file << " Function: " << function << " Line: " << line << std::endl;
	}
};

int main(int argc, char* args[]) {
	LogHandle h;
	pxengine::PxeEngineBase* engineBase = pxengine::createPXEEngineBase(h);
	pxengine::PxeWindow* window = engineBase->createWindow(1000, 1000, "Test Window");
	if (window) {
		window->setSwapInterval(1);
		while (!window->getShouldClose())
		{
			window->acquireGlContext();
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			window->swapFrameBuffer();
			window->releaseGlContext();
			window->pollEvents();
		}

		window->drop();
	}

	engineBase->drop();
	return 0;
}