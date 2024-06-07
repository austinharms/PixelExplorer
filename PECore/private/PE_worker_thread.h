#ifndef PE_WORKER_THREAD_H_
#define PE_WORKER_THREAD_H_
#include "PE_defines.h"
#include "PE_work_queue.h"
#include "PE_errors.h"
#include <thread>

namespace pe::internal {
	// class that wraps a std::thread and allows queuing "work" to run on the thread
	class ThreadWorker : protected WorkQueue {
	public:
		ThreadWorker();
		~ThreadWorker();
		// Create the worker's thread and begins processing the work queue
		// Note: this operation is not thread safe
		void Start();
		// Waits for all pending work to be completed and destroys the worker's thread
		// Note: this operation is not thread safe
		void Stop() PE_NOEXCEPT;
		// Returns true if the worker is running
		// Note: running in this case means the worker has a thread NOT if it is performing work
		PE_NODISCARD bool GetRunning() const PE_NOEXCEPT;
		using WorkQueue::PushAsyncWork;
		using WorkQueue::PushBlockingWork;
		using WorkQueue::PushBlockingTWork;

	private:
		std::thread worker_thread_;
		bool enable_flag_;
		bool has_thread_;

		void ThreadWorkerEntry();
	};
}
#endif // !PE_WORKER_THREAD_H_
