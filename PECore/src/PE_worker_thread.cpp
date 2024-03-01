#include "PE_worker_thread.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_log.h"

namespace pecore {
	enum class ThreadWorkTypeEnum
	{
		PE_THREAD_WORK_TYPE_DEFAULT,
		PE_THREAD_WORK_TYPE_BLOCKING,
	};

	struct ThreadWorker::WorkDataBase {
		WorkDataBase* next;
		WorkerFunction function;
		void* userdata;
		ThreadWorkTypeEnum work_type;
	};

	struct ThreadWorker::WorkData : public WorkDataBase
	{
		WorkData() {
			next = nullptr;
			work_type = ThreadWorkTypeEnum::PE_THREAD_WORK_TYPE_DEFAULT;
		}
	};

	struct ThreadWorker::BlockingWorkData : public WorkDataBase {
		std::mutex result_mutex;
		std::condition_variable result_condition;
		void* result;
		bool function_executed;

		BlockingWorkData() {
			next = nullptr;
			function_executed = false;
			work_type = ThreadWorkTypeEnum::PE_THREAD_WORK_TYPE_BLOCKING;
			result = nullptr;
		}
	};

	ThreadWorker::ThreadWorker() :
		enable_flag_(true),
		work_head_(nullptr),
		work_tail_(nullptr),
		worker_thread_(&ThreadWorker::ThreadWorkerEntry, this) {
	}

	ThreadWorker::~ThreadWorker()
	{
		enable_flag_ = false;
		work_condition_.notify_all();
		worker_thread_.join();
	}

	int ThreadWorker::Run(WorkerFunction function, void* userdata)
	{
		WorkData* work_data = static_cast<WorkData*>(PE_malloc(sizeof(WorkData)));
		if (work_data == nullptr) {
			return PE_ERROR_OUT_OF_MEMORY;
		}

		work_data->function = function;
		work_data->userdata = userdata;
		std::lock_guard worker_lock(work_mutex_);
		if (work_tail_) {
			work_tail_->next = work_data;
		}
		else {
			work_head_ = work_data;
		}

		work_tail_ = work_data;
		return PE_ERROR_NONE;
	}

	void* ThreadWorker::RunBlocking(WorkerFunction function, void* userdata)
	{
		BlockingWorkData work_data;
		work_data.function = function;
		work_data.userdata = userdata;
		std::unique_lock work_lock(work_data.result_mutex);
		{
			std::lock_guard worker_lock(work_mutex_);
			if (work_tail_) {
				work_tail_->next = &work_data;
			}
			else {
				work_head_ = &work_data;
			}

			work_tail_ = &work_data;
		}

		work_condition_.notify_all();
		work_data.result_condition.wait(work_lock, [&] { return work_data.function_executed; });
		return work_data.result;
	}

	void ThreadWorker::ThreadWorkerEntry()
	{
		PE_DEBUG_ASSERT(enable_flag_, PE_TEXT("ThreadWorker enable_flag_ was false during start"));
		// Ensure all work is completed before exiting
		// Note: if there is recursive work this thread may never exit and cause a softlock
		bool has_work = false;
		while (enable_flag_ || has_work)
		{
			has_work = false;
			std::unique_lock lock(work_mutex_);
			work_condition_.wait(lock, [&] { return work_head_ != nullptr || !enable_flag_; });
			if (work_head_ != nullptr) {
				has_work = true;
				WorkDataBase* work = work_head_;
				if (work_tail_ == work_head_) {
					work_tail_ = nullptr;
				}

				work_head_ = work->next;
				lock.unlock();
				void* res = work->function(work->userdata);
				switch (work->work_type)
				{
				case ThreadWorkTypeEnum::PE_THREAD_WORK_TYPE_BLOCKING: {
					BlockingWorkData* blocking_work = static_cast<BlockingWorkData*>(work);
					blocking_work->result = res;
					blocking_work->result_mutex.lock();
					blocking_work->function_executed = true;
					blocking_work->result_mutex.unlock();
					blocking_work->result_condition.notify_all();
				} break;

				case ThreadWorkTypeEnum::PE_THREAD_WORK_TYPE_DEFAULT:
					PE_free(work);
					break;

				default:
					PE_DEBUG_ASSERT(false, PE_TEXT("Unknown ThreadWorkTypeEnum value"));
					break;
				}
			}
		}
	}
}
