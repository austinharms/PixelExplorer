#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_events.h"

namespace pecore::event_loop {
	namespace {
		std::aligned_storage<sizeof(WorkQueue), alignof(WorkQueue)>::type queue_storage;
		WorkQueue* work_queue;
		bool loop_flag;
	}

	void Prepare() {
		if (loop_flag) {
			return;
		}

		work_queue = new(&queue_storage) WorkQueue();
		loop_flag = true;
	}

	void Start() {
		if (!loop_flag) {
			PE_LogWarn(PE_LOG_CATEGORY_EVENT, PE_TEXT("You must call event_loop::Prepare before calling event_loop::Start"));
			return;
		}

		work_queue->WorkerEntry();
		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
		bool has_work = false;
		while (loop_flag || has_work)
		{
			SDL_PumpEvents();
			has_work = work_queue->PerformWork();
		}

		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
		work_queue->WorkerExit();
	}

	void Stop() {
		loop_flag = false;
		work_queue->~WorkQueue();
		work_queue = nullptr;
	}

	int RunWork(PE_WorkFunction work, void* userdata) {
		return work_queue->PushAsyncWork(work, userdata);
	}

	void RunWorkBlocking(PE_WorkFunction work, void* userdata) {
		work_queue->PushBlockingWork(work, userdata);
	}
}
