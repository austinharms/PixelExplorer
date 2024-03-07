#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"

namespace pecore {
	void EventLoop::Prepare() {
		if (s_loop_flag_) {
			return;
		}

		s_work_queue_ = new(&s_queue_storage_) WorkQueue();
		s_loop_flag_ = true;
	}

	void EventLoop::Start() {
		if (!s_loop_flag_) {
			PE_LogWarn(PE_LOG_CATEGORY_EVENT, PE_TEXT("You must call EventLoop::Prepare before calling EventLoop::Start"));
			return;
		}

		s_work_queue_->WorkerEntry();
		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
		bool has_work = false;
		while (s_loop_flag_ || has_work)
		{
			SDL_PumpEvents();
			has_work = s_work_queue_->PerformWork();
		}

		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
		s_work_queue_->WorkerExit();
	}

	void EventLoop::Stop() {
		s_loop_flag_ = false;
		s_work_queue_->~WorkQueue();
		s_work_queue_ = nullptr;
	}

	int EventLoop::RunWork(PE_WorkFunction work, void* userdata) {
		return s_work_queue_->PushAsyncWork(work, userdata);
	}

	void EventLoop::RunWorkBlocking(PE_WorkFunction work, void* userdata) {
		s_work_queue_->PushBlockingWork(work, userdata);
	}
}
