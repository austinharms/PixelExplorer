#ifndef PE_WORKER_THREAD_H_
#define PE_WORKER_THREAD_H_
#include "PE_defines.h"
#include "PE_work_queue.h"
#include <thread>

namespace pecore {
	// class that wraps a std::thread and allows queuing "work" to run on the thread
	class ThreadWorker {
	public:
		ThreadWorker();
		~ThreadWorker();

		// Runs function on the worker and returns immediately, returns 0 on success
		int Run(PE_WorkFunction function, void* userdata = nullptr);

		// Runs function on the worker and waits for the function to completed
		void RunBlocking(PE_WorkFunction function, void* userdata = nullptr);

	private:
		WorkQueue work_queue_;
		std::thread worker_thread_;
		bool enable_flag_;

		void ThreadWorkerEntry();
	};
}
#endif // !PE_WORKER_THREAD_H_
