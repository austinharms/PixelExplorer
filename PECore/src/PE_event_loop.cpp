#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"
#include <thread>

namespace {
	bool l_eventThreadFlag = false;
	//void startEventThread() {
	//	if (!l_eventThreadFlag) {
	//		if (l_eventThread.joinable()) {
	//			l_eventThread.join();
	//		}

	//		l_eventThread = std::move(std::thread(eventThreadEntry));
	//		l_eventThreadFlag = true;
	//	}
	//	else {
	//		PE_LogWarn(PE_LOG_CATEGORY_EVENT, PE_TEXT("Attempted to start event thread when one was already running"));
	//	}
	//}
}

PE_EXTERN_C PE_API void PE_CALL PE_PrepareSDLEventLoop()
{
	l_eventThreadFlag = true;
}

PE_EXTERN_C PE_API void PE_CALL PE_RunSDLEventLoop() {
	PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
	while (l_eventThreadFlag)
	{
		SDL_PumpEvents();
	}

	PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
}

PE_EXTERN_C PE_API void PE_CALL PE_StopSDLEventLoop()
{
	l_eventThreadFlag = false;
}
