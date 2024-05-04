#ifndef PE_GLOBAL_H_
#define PE_GLOBAL_H_
#include "PE_event_loop.h"

namespace pe::internal {
	extern struct GlobalStruct {
		EventLoop event_loop;
	} Global;
}
#endif // !PE_GLOBAL_H_