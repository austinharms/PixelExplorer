#include <iostream>
#include <cassert>
#include <thread>

#include "PxeEngine.h"
#include "PxeRingBuffer.h"
#include "PxeWindow.h"
#include "SDL.h"
#include "GL/glew.h"


class LogHandle : public pxengine::PxeLogHandler {
public:
	virtual ~LogHandle() {}
	void onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function) override {
		pxengine::LogLevel l = (pxengine::LogLevel)level;
		switch (l)
		{
		case pxengine::LogLevel::INFO:
			std::cout << "[INFO] ";
			break;
		case pxengine::LogLevel::WARN:
			std::cout << "[WARN] ";
			break;
		case pxengine::LogLevel::ERROR:
			std::cout << "[ERROR] ";
			break;
		case pxengine::LogLevel::FATAL:
			std::cout << "[FATAL] ";
			break;
		}

		//std::cout << "Log: " << msg << " Level: " << level << " File: " << file << " Function: " << function << " Line: " << line << std::endl;
		std::cout << msg << std::endl;
	}
};

void runWindow(pxengine::PxeEngineBase* engineBase, uint32_t width, uint32_t height, const char* title, float r, float g, float b) {
	pxengine::PxeWindow* window = engineBase->createWindow(width, height, title);
	pxengine::PxeScene* scene = engineBase->createScene();
	assert(window);
	assert(scene);
	window->setScene(scene);
	window->setSwapInterval(1);
	while (!window->getShouldClose())
	{
		// 1 should be delta time
		scene->simulatePhysics(1);
		window->acquireGlContext();
		window->setWindowHidden(false);
		//std::cout << title << " Draw" << std::endl;
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window->drawScene();
		window->swapFrameBuffer();
		window->pollEvents();
		window->releaseGlContext();
	}

	window->drop();
}

int main(int argc, char* args[]) {
	LogHandle h;
	pxengine::PxeEngineBase* engineBase = pxengine::createPXEEngineBase(h);
	assert(engineBase);
	std::thread win1(runWindow, engineBase, 500, 500, "Win 1", 1, 0, 0);
	std::thread win2(runWindow, engineBase, 500, 500, "Win 2", 0, 1, 0);
	win1.join();
	win2.join();
	engineBase->drop();
	return 0;
}
