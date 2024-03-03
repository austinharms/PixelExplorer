#include "PE_event_loop.h"
#include "PE_log.h"
#include "PE_memory.h"
#include "SDL_events.h"
#include "PE_errors.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace pecore {
	namespace {
		std::mutex loop_sync_mutex;
		std::aligned_storage<sizeof(WorkQueue), alignof(WorkQueue)>::type work_queue_storage;
		WorkQueue* work_queue = nullptr;
		bool event_loop_flag = false;
	}

	void PE_PrepareSDLEventLoop()
	{
		work_queue = new(&work_queue_storage) WorkQueue();
		event_loop_flag = true;
	}

	void PE_RunSDLEventLoop() {
		loop_sync_mutex.lock();
		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Entry"));
		// Ensure all work is completed before exiting
		// Note: if there is recursive work this thread may never exit and cause a softlock
		bool has_work = false;
		while (event_loop_flag || has_work)
		{
			SDL_PumpEvents();
			has_work = work_queue->PerformWork();
		}

		PE_LogDebug(PE_LOG_CATEGORY_EVENT, PE_TEXT("Event Thread Exit"));
		loop_sync_mutex.unlock();
	}

	void PE_StopSDLEventLoop()
	{
		work_queue->ForceWaitWakeup();
		event_loop_flag = false;
		// Ensure loop has exited before proceeding
		loop_sync_mutex.lock();
		loop_sync_mutex.unlock();
		work_queue->~WorkQueue();
		work_queue = nullptr;
	}

	void PE_RunEventLoopFunctionBlocking(PE_WorkFunction fn, void* userdata)
	{
		work_queue->PushBlockingWork(fn, userdata);
	}

	int PE_RunEventLoopFunction(PE_WorkFunction fn, void* userdata)
	{
		return work_queue->PushAsyncWork(fn, userdata);
	}
}
