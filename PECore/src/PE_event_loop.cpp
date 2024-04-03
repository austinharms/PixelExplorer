#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"

namespace pecore {
	EventLoopClass PE_EventLoop;

	void EventLoopClass::Start() {
		run_flag_ = true;
		WorkQueue::WorkerEntry();
		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
		bool has_work = false;
		while (run_flag_ || has_work)
		{
			SDL_PumpEvents();
			has_work = WorkQueue::PerformWork();
		}

		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
		WorkQueue::WorkerExit();
	}

	void EventLoopClass::Stop() {
		run_flag_ = false;
		WorkQueue::ForceWaitWakeup();
	}
}
