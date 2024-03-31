#include "PE_core.h"
#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL.h"
#include <thread>

namespace pecore {
	namespace {
		// entry point for the application thread
		static void PE_app_main(int* returnValue, int argc, char** argv) {
			PE_LogDebug(PE_LOG_CATEGORY_CORE, PE_TEXT("PE_main Thread Entry"));
			// TODO Add main loop

			event_loop::Stop();
			*returnValue = 0;
			PE_LogDebug(PE_LOG_CATEGORY_CORE, PE_TEXT("PE_main Thread Exit"));
		}
	}

	// main entry point for the application
	int PE_main(int argc, char** argv)
	{
		constexpr Uint32 SDL_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
		PE_InitLog();
		int rtn = SDL_Init(SDL_SYSTEMS);
		PE_ASSERT(rtn == 0, PE_TEXT("Failed to init SDL %") SDL_PRIs32, rtn);
		event_loop::Prepare();
		// Run the application on a different thread as events must be polled on the main thread
		// and there is a bug when resizing windows will freeze the event loop
		std::thread appThread(PE_app_main, &rtn, argc, argv);
		event_loop::Start();
		appThread.join();
		return rtn;
	}
}
