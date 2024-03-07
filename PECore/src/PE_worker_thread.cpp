#include "PE_worker_thread.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_log.h"

namespace pecore {
	ThreadWorker::ThreadWorker() :
		enable_flag_(true),
		worker_thread_(&ThreadWorker::ThreadWorkerEntry, this) {
	}

	ThreadWorker::~ThreadWorker()
	{
		enable_flag_ = false;
		work_queue_.ForceWaitWakeup();
		worker_thread_.join();
	}

	int ThreadWorker::Run(PE_WorkFunction function, void* userdata)
	{
		return work_queue_.PushAsyncWork(function, userdata);
	}

	void ThreadWorker::RunBlocking(PE_WorkFunction function, void* userdata)
	{
		work_queue_.PushBlockingWork(function, userdata);
	}

	void ThreadWorker::ThreadWorkerEntry()
	{
		work_queue_.WorkerEntry();
		PE_DEBUG_ASSERT(enable_flag_, PE_TEXT("ThreadWorker enable_flag_ was false during start"));
		// Ensure all work is completed before exiting
		// Note: if there is recursive work this thread may never exit and cause a softlock
		bool has_work = false;
		while (enable_flag_ || has_work)
		{
			if (enable_flag_) {
				work_queue_.WaitForWork();
			}

			has_work = work_queue_.PerformWork();
		}

		work_queue_.WorkerExit();
	}
}
