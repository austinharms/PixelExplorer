#include "PECore.h"
#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL.h"
#include <thread>

namespace {
	// entry point for the application thread
	void PE_CALL PE_main(int* returnValue, int argc, char** argv) {
		PE_LogDebug(PE_LOG_CATEGORY_CORE, PE_TEXT("PE_main Thread Entry"));
		// TODO Add main loop

		PE_StopSDLEventLoop();
		*returnValue = 0;
		PE_LogDebug(PE_LOG_CATEGORY_CORE, PE_TEXT("PE_main Thread Exit"));
	}
}

// main entry point for the application
PE_EXTERN_C PE_API int PE_CALL PECORE_main(int argc, char** argv)
{
	constexpr Uint32 SDL_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	PE_InitLog();
	int returnRes = SDL_Init(SDL_SYSTEMS);
	PE_ASSERT(returnRes == 0, "Failed to init SDL " SDL_PRIs32, returnRes);
	PE_PrepareSDLEventLoop();
	// Run the application on a different thread as events must be polled on the main thread
	// and there is a bug when resizing windows will freeze the event loop
	std::thread appThread(PE_main, &returnRes, argc, argv);
	PE_RunSDLEventLoop();
	appThread.join();
	return returnRes;
}
