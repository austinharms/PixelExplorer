#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"

namespace pe::internal {
	void EventLoop::Start() {
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

	void EventLoop::Stop() {
		run_flag_ = false;
		WorkQueue::ForceWaitWakeup();
	}
}
