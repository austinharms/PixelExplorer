#ifndef PE_WORK_QUEUE_H_
#define PE_WORK_QUEUE_H_
#include "PE_defines.h"
#include "PE_barrier.h"
#include <mutex>
#include <condition_variable>

namespace pecore {
	typedef void (*PE_WorkFunction)(void*);

	// A FIFO queue of work to be performed on a different thread
	// Note: this only expects one "consumer" thread to exist and ForceWaitWakeup may break with multiple threads
	class WorkQueue {
	public:
		struct Work {
			Work* next;
			PE_WorkFunction function;
			void* userdata;
			bool blocking;
		};

		struct WorkBarrier : public Barrier, public Work {
			WorkBarrier(size_t count) : Barrier(count) {}
		};

		WorkQueue();
		~WorkQueue();
		// Push work onto the work queue, returns immediately
		int PushAsyncWork(PE_WorkFunction function, void* userdata = nullptr);

		// Push work that will block until the work is completed
		void PushBlockingWork(PE_WorkFunction function, void* userdata = nullptr);

		// Push work that will call WorkBarrier.Trigger() on completion
		// You must call WorkBarrier.Wait() at some point after calling this method
		void PushBlockingWork(PE_WorkFunction function, void* userdata, WorkBarrier& barrier);

		// Returns true if there is work to be done
		PE_NODISCARD bool HasWork();

		// Blocks until work is pushed onto the queue
		void WaitForWork();

		// Forces Wait for work to "wakeup" and stop blocking
		void ForceWaitWakeup();

		// Takes the first work item from the queue and runs it and returns true, returns false if no work was available
		bool PerformWork();

		// Should be called on the worker thread before performing work
		// this ensures only one worker is running at a time
		void WorkerEntry();

		// Should be called on the worker thread once it will no longer perform work
		// this ensures the WorkQueue will not be destroyed before the worker exits
		void WorkerExit();

	private:
		Work* work_head_;
		Work* work_tail_;
		std::mutex work_mutex_;
		std::mutex worker_sync_mutex_;
		std::condition_variable work_condition_;
		bool force_wake_;

		void PushWork(Work* work);
	};
}
#endif // !PE_WORK_QUEUE_H_