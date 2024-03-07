#ifndef PE_EVENT_LOOP_H_
#define PE_EVENT_LOOP_H_
#include "PE_defines.h"
#include "PE_work_queue.h"
#include <type_traits> // std::aligned_storage

namespace pecore {
	class EventLoop PE_FINAL {
	public:
		EventLoop() = delete;
		~EventLoop() = delete;

		// Sets a flag allowing the event loop to execute
		static void Prepare();

		// Runs the event loop
		// Note: this will not return until Stop() is called or if Prepare() was not called returns immediately
		static void Start();

		// Stops the event loop
		// Note: this does not wait for the loop to stop
		static void Stop();

		// Runs the provided function on the event loop thread, passes userdata as the function parameter returns 0 on success
		// this requires the event thread to be running
		static int RunWork(PE_WorkFunction work, void* userdata = nullptr);

		// Runs the provided function on the event loop thread, passes userdata as the function parameter
		// Note: this requires the event thread to be running and will block until the function is executed
		static void RunWorkBlocking(PE_WorkFunction work, void* userdata = nullptr);

	private:
		static std::aligned_storage<sizeof(WorkQueue), alignof(WorkQueue)>::type s_queue_storage_;
		static WorkQueue* s_work_queue_;
		static bool s_loop_flag_;
	};
}
#endif // !PE_EVENT_LOOP_H_
