#include "PE_log.h"
#include "PE_event_loop.h"
#include "PE_graphics.h"
#include "SDL.h"
#include <thread>

using namespace pecore;

void PET_main(int* returnValue, int argc, char** argv) {
	graphics::Shader* shader = nullptr;
	for (int i = 0; i < 10; ++i) {
		SDL_Window* window = graphics::CreateWindow(PE_TEXT("Test Window"), 1000, 800, 0);
		PE_ASSERT(window, PE_TEXT("Failed to create SDL window"));
		if (i == 0) {
			shader = graphics::LoadShader("test");
			PE_ASSERT(shader, PE_TEXT("Failed to load test shader"));
		}

		graphics::DestroyWindow(window);
	}
	
	graphics::UnloadShader(shader);
	event_loop::Stop();
	*returnValue = 0;
}

int main(int argc, char** argv)
{
	constexpr Uint32 SDL_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	PE_InitLog();
	PE_LogInfo(PE_LOG_CATEGORY_TEST, PE_TEXT("Hello World"));
	int returnRes = SDL_Init(SDL_SYSTEMS);
	PE_ASSERT(returnRes == 0, PE_TEXT("Failed to init SDL %") SDL_PRIs32, returnRes);
	returnRes = graphics::Init();
	PE_ASSERT(returnRes == 0, PE_TEXT("Failed to init PE_Graphics %") SDL_PRIs32, returnRes);
	event_loop::Prepare();
	std::thread appThread(PET_main, &returnRes, argc, argv);
	event_loop::Start();
	appThread.join();
	graphics::Quit();
	return returnRes;
}
