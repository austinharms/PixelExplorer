#include "PE_worker_thread.h"
#include "PE_memory.h"
#include "PE_exception.h"
#include "PE_log.h"
// worker_thread_(&ThreadWorker::ThreadWorkerEntry, this)
namespace pe::internal {
	ThreadWorker::ThreadWorker() :
		enable_flag_(false),
		has_thread_(false),
		worker_thread_(std::thread::id()) {
	}

	ThreadWorker::~ThreadWorker()
	{
		Stop();
	}

	void ThreadWorker::Start()
	{
		if (has_thread_) {
			throw ErrorCodeException(PE_ERROR_ALREADY_INITIALIZED);
		}

		enable_flag_ = true;
		try {
			worker_thread_ = std::move(std::thread(&ThreadWorker::ThreadWorkerEntry, this));
		}
		catch (...) {
			enable_flag_ = false;
			throw;
		}

		has_thread_ = true;
	}

	void ThreadWorker::Stop() PE_NOEXCEPT
	{
		if (has_thread_ == false) {
			return;
		}

		enable_flag_ = false;
		WorkQueue::ForceWaitWakeup();

		try {
			worker_thread_.join();
		}
		catch (...) {
			CatchAllExceptionLogger();
		}

		has_thread_ = false;
	}

	PE_NODISCARD bool ThreadWorker::GetRunning() const PE_NOEXCEPT
	{
		return has_thread_;
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
			has_work = false;
			try {
				if (enable_flag_) {
					WorkQueue::WaitForWork();
				}

				has_work = WorkQueue::PerformWork();
			}
			catch (...) {
				CatchAllExceptionLogger();
			}
		}

		WorkQueue::WorkerExit();
	}
}
