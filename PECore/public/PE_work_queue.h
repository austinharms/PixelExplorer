#ifndef PE_WORK_QUEUE_H_
#define PE_WORK_QUEUE_H_
#include "PE_defines.h"
#include <mutex>
#include <condition_variable>

namespace pecore {
	typedef void (*PE_WorkFunction)(void*);

	// A FIFO queue of work to be performed on a different thread
	// Note: this only expects one "consumer" thread to exist and ForceWaitWakeup may break with multipule threads
	class WorkQueue final {
	public:
		WorkQueue();
		~WorkQueue();
		int PushAsyncWork(PE_WorkFunction function, void* userdata = nullptr);
		void PushBlockingWork(PE_WorkFunction function, void* userdata = nullptr);

		// Returns true if there is work to be done
		PE_NODISCARD bool HasWork();

		// Blocks until work is pushed onto the queue
		void WaitForWork();

		// Forces Wait for work to "wakeup" and stop blocking
		void ForceWaitWakeup();

		// Takes the first work item from the queue and runs it and returns true, returns false if no work was available
		bool PerformWork();

	private:
		struct WorkBase;
		struct AsyncWork;
		struct BlockingWork;

		WorkBase* work_head_;
		WorkBase* work_tail_;
		std::mutex work_mutex_;
		std::condition_variable work_condition_;

		static void DummyWork(void*);
		void PushWork(WorkBase* work);
	};
}
#endif // !PE_WORK_QUEUE_H_
