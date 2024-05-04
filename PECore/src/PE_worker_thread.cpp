#include "PE_worker_thread.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_log.h"

namespace pe::internal {
	ThreadWorker::ThreadWorker() :
		enable_flag_(true),
		worker_thread_(&ThreadWorker::ThreadWorkerEntry, this) {
	}

	ThreadWorker::~ThreadWorker()
	{
		enable_flag_ = false;
		WorkQueue::ForceWaitWakeup();
		worker_thread_.join();
	}

	void ThreadWorker::ThreadWorkerEntry()
	{
		WorkQueue::WorkerEntry();
		PE_DEBUG_ASSERT(enable_flag_, PE_TEXT("ThreadWorker enable_flag_ was false during start"));
		// Ensure all work is completed before exiting
		// Note: if there is recursive work this thread may never exit and cause a softlock
		bool has_work = false;
		while (enable_flag_ || has_work)
		{
			if (enable_flag_) {
				WorkQueue::WaitForWork();
			}

			has_work = WorkQueue::PerformWork();
		}

		WorkQueue::WorkerExit();
	}
}
