#ifndef PE_GLOBAL_H_
#define PE_GLOBAL_H_
#include "PE_event_loop.h"
#include "PE_graphics_implementation.h"

namespace pe::internal {
	struct GlobalData {
		EventLoop event_loop;
		GraphicsImplementation graphics;
	};

	PE_NODISCARD extern GlobalData& GetGlobalData();
}
#endif // !PE_GLOBAL_H_