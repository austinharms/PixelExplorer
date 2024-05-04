#include "PE_application_entry.h"
#include "PE_global.h"
#include "PE_event_loop.h"
#include "PE_log.h"
#include "PE_log_init.h"
#include "SDL.h"
#include <thread>

namespace pe::internal {
	static void application_thread_entry(int* returnValue, int argc, char** argv) {
		PE_LogDebug(PE_LOG_CATEGORY_CORE, PE_TEXT("PE_main Thread Entry"));
		// TODO Add main loop

		Global.event_loop.Stop();
		*returnValue = 0;
		PE_LogDebug(PE_LOG_CATEGORY_CORE, PE_TEXT("PE_main Thread Exit"));
	}

	int application_entry(int argc, char** argv)
	{
		constexpr Uint32 SDL_SYSTEMS = SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
		InitLog();
		int exit_code = SDL_Init(SDL_SYSTEMS);
		PE_ASSERT(exit_code == 0, PE_TEXT("Failed to init SDL %") SDL_PRIs32, exit_code);
		// Run the application on a different thread as events must be polled on the main thread
		// and there is a bug when resizing windows will freeze the event loop
		std::thread appThread(application_thread_entry, &exit_code, argc, argv);
		Global.event_loop.Start();
		appThread.join();
		return exit_code;
	}
}
