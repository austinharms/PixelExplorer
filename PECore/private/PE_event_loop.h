#ifndef PE_EVENT_LOOP_H_
#define PE_EVENT_LOOP_H_
#include "PE_defines.h"
#include "PE_work_queue.h"
#include <type_traits> // std::aligned_storage

namespace pecore::event_loop {
	// Sets a flag allowing the event loop to execute
	void PE_CALL Prepare();

	// Runs the event loop
	// Note: this will not return until Stop() is called or if Prepare() was not called returns immediately
	void PE_CALL Start();

	// Stops the event loop
	// Note: this does not wait for the loop to stop
	void PE_CALL Stop();

	// Runs the provided function on the event loop thread, passes userdata as the function parameter returns 0 on success
	// this requires the event thread to be running
	int PE_CALL RunWork(PE_WorkFunction work, void* userdata = nullptr);

	// Runs the provided function on the event loop thread, passes userdata as the function parameter
	// Note: this requires the event thread to be running and will block until the function is executed
	void PE_CALL RunWorkBlocking(PE_WorkFunction work, void* userdata = nullptr);
}
#endif // !PE_EVENT_LOOP_H_
