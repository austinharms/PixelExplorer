#ifndef PE_WORKER_THREAD_H_
#define PE_WORKER_THREAD_H_
#include "PE_defines.h"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace pecore {
	// class that wraps a std::thread and allows queuing "work" to run on the thread
	class ThreadWorker {
	public:
		typedef void* (*WorkerFunction)(void*);

		ThreadWorker();
		~ThreadWorker();

		// Runs function on the worker and returns immediately discarding the return value
		// Returns 0 on success
		int Run(WorkerFunction function, void* userdata = nullptr);

		// Runs function on the worker and waits for the function to completed and returns the result
		void* RunBlocking(WorkerFunction function, void* userdata = nullptr);

	private:
		struct WorkDataBase;
		struct WorkData;
		struct BlockingWorkData;

		bool enable_flag_;
		WorkDataBase* work_head_;
		WorkDataBase* work_tail_;
		std::thread worker_thread_;
		std::mutex work_mutex_;
		std::condition_variable work_condition_;

		void ThreadWorkerEntry();
	};
}
#endif // !PE_WORKER_THREAD_H_
