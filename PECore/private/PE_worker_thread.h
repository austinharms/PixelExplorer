#ifndef PE_WORKER_THREAD_H_
#define PE_WORKER_THREAD_H_
#include "PE_defines.h"
#include "PE_work_queue.h"
#include <thread>

namespace pecore {
	// class that wraps a std::thread and allows queuing "work" to run on the thread
	class ThreadWorker : protected WorkQueue {
	public:
		ThreadWorker();
		~ThreadWorker();
		using WorkQueue::PushAsyncWork;
		using WorkQueue::PushBlockingWork;
		using WorkQueue::PushBlockingTWork;

	private:
		std::thread worker_thread_;
		bool enable_flag_;

		void ThreadWorkerEntry();
	};
}
#endif // !PE_WORKER_THREAD_H_
