#ifndef PE_EVENT_LOOP_H_
#define PE_EVENT_LOOP_H_
#include "PE_defines.h"
#include "PE_work_queue.h"

namespace pecore {
	class EventLoopClass : protected WorkQueue {
	public:
		EventLoopClass() = default;
		// Runs the event loop, this will not return until Stop() is called
		void Start();
		// Stops the event loop
		void PE_CALL Stop();
		// Runs the provided function on the event loop thread
		using WorkQueue::PushAsyncWork;
		// Runs the provided function on the event loop thread
		using WorkQueue::PushBlockingWork;
		// Runs the provided function on the event loop thread
		using WorkQueue::PushBlockingTWork;

	private:
		bool run_flag_;
	};

	extern EventLoopClass PE_EventLoop;
}
#endif // !PE_EVENT_LOOP_H_
