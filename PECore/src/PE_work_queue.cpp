#include "PE_work_queue.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_log.h"

namespace pecore {
	WorkQueue::WorkQueue()
	{
		work_head_ = nullptr;
		work_tail_ = nullptr;
		force_wake_ = false;
	}

	WorkQueue::~WorkQueue()
	{
		ForceWaitWakeup();
		// Ensure worker has exited
		worker_sync_mutex_.lock();
		worker_sync_mutex_.unlock();
		work_mutex_.lock();
		if (work_tail_ || work_head_) {
			PE_LogError(PE_LOG_CATEGORY_CORE, PE_TEXT("WorkQueue destroyed with work pending"));
		}

		work_mutex_.unlock();
	}

	int WorkQueue::PushAsyncWork(PE_WorkFunction function, void* userdata)
	{
		Work* work = static_cast<Work*>(PE_malloc(sizeof(Work)));
		if (!work) {
			return PE_ERROR_OUT_OF_MEMORY;
		}

		work->blocking = false;
		work->next = nullptr;
		work->function = function;
		work->userdata = userdata;
		PushWork(work);
		return PE_ERROR_NONE;
	}

	void WorkQueue::PushBlockingWork(PE_WorkFunction function, void* userdata)
	{
		WorkBarrier work(2);
		work.blocking = true;
		work.next = nullptr;
		work.function = function;
		work.userdata = userdata;
		PushWork(&work);
		work.Wait();
	}

	PE_NODISCARD bool WorkQueue::HasWork()
	{
		std::lock_guard lock(work_mutex_);
		return work_head_ != nullptr;
	}

	void WorkQueue::WaitForWork()
	{
		std::unique_lock lock(work_mutex_);
		work_condition_.wait(lock, [&] { return work_head_ != nullptr || force_wake_; });
		force_wake_ = false;
	}

	void WorkQueue::ForceWaitWakeup()
	{
		force_wake_ = true;
		work_condition_.notify_one();
	}

	bool WorkQueue::PerformWork()
	{
		std::unique_lock lock(work_mutex_);
		if (work_head_) {
			Work* work = work_head_;
			if (work_tail_ == work_head_) {
				work_tail_ = nullptr;
			}

			work_head_ = work->next;
			lock.unlock();
			work->function(work->userdata);
			if (work->blocking) {
				WorkBarrier* barrier = static_cast<WorkBarrier*>(work);
				barrier->Trigger();
			}
			else {
				PE_free(work);
			}

			return true;
		}
		else {
			return false;
		}
	}

	void WorkQueue::WorkerEntry() {
		// Ensure only one worker by locking the worker mutex here and releasing it in WorkerExit()
		worker_sync_mutex_.lock();
	}

	void WorkQueue::WorkerExit() {
		worker_sync_mutex_.unlock();
	}

	void WorkQueue::PushWork(Work* work)
	{
		work_mutex_.lock();
		if (work_tail_) {
			work_tail_->next = work;
		}
		else {
			work_head_ = work;
		}

		work_tail_ = work;
		work_mutex_.unlock();
		work_condition_.notify_one();
	}
}
