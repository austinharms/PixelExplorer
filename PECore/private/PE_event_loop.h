#ifndef PE_EVENT_LOOP_H_
#define PE_EVENT_LOOP_H_
#include "PE_defines.h"
#include "PE_work_queue.h"

namespace pecore {
	void PE_CALL PE_PrepareSDLEventLoop();
	void PE_CALL PE_RunSDLEventLoop();
	void PE_CALL PE_StopSDLEventLoop();

	// Runs the provided function on the event loop thread, passes userdata as the function parameter
	// Note: this requires the event thread to be running and will block until the function is executed
	void PE_CALL PE_RunEventLoopFunctionBlocking(PE_WorkFunction fn, void* userdata = nullptr);

	// Runs the provided function on the event loop thread, passes userdata as the function parameter returns 0 on success
	int PE_CALL PE_RunEventLoopFunction(PE_WorkFunction fn, void* userdata = nullptr);
}
#endif // !PE_EVENT_LOOP_H_
