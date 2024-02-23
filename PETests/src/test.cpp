#include "PE_log.h"
#include "PE_event_loop.h"
#include "PE_graphics_adapter.h"
#include "PE_graphics.h"
#include "SDL.h"
#include <thread>

//void* PE_CALL PET_CreateWindow(void*) {
//	return SDL_CreateWindow(PE_TEXT("Test Window"), 1000, 800, 0);
//}
//
//void* PE_CALL PET_DestroyWindow(void* window) {
//	SDL_DestroyWindow(static_cast<SDL_Window*>(window));
//	return nullptr;
//}

void PET_main(int* returnValue, int argc, char** argv) {
	for (int i = 0; i < 100; ++i) {
		//SDL_Window* window = static_cast<SDL_Window*>(PE_RunEventLoopFunction(PET_CreateWindow));
		SDL_Window* window = PE_CreateWindow(PE_TEXT("Test Window"), 1000, 800, 0);
		PE_ASSERT(window, PE_TEXT("Failed to create SDL window"));
		PE_DestroyWindow(window);
		//PE_RunEventLoopFunction(PET_DestroyWindow, window);
	}

	PE_StopSDLEventLoop();
	*returnValue = 0;
}

int main(int argc, char** argv)
{
	constexpr Uint32 SDL_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	PE_InitLog();
	PE_LogInfo(PE_LOG_CATEGORY_TEST, PE_TEXT("Hello World"));
	int returnRes = SDL_Init(SDL_SYSTEMS);
	PE_ASSERT(returnRes == 0, PE_TEXT("Failed to init SDL %") SDL_PRIs32, returnRes);
	returnRes = PE_InitGraphicsAdapter();
	PE_ASSERT(returnRes == 0, PE_TEXT("Failed to init PE_Graphics %") SDL_PRIs32, returnRes);
	PE_PrepareSDLEventLoop();
	std::thread appThread(PET_main, &returnRes, argc, argv);
	PE_RunSDLEventLoop();
	appThread.join();
	return returnRes;
	return 0;
}
