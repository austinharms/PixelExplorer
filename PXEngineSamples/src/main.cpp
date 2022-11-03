#include <iostream>

#include "PxeEngine.h"
#include "SDL.h"

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
	std::cout << engineBase->testFn(5) << std::endl;
	SDL_Delay(5000);
	engineBase->drop();
	return 0;
}