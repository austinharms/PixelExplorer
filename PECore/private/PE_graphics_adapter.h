#ifndef PE_GRAPHICS_ADAPTER_H_
#define PE_GRAPHICS_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics_types.h"

namespace pecore::pe_graphics {
	class GraphicsAdapterInterface {
	public:
		GraphicsAdapterInterface(int* ret_val) {}
		virtual ~GraphicsAdapterInterface() = default;
#define PE_GRAPHICS_API(rc, fn, params, args, ret) virtual rc fn params = 0;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
	};
}
#endif // !PE_GRAPHICS_ADAPTER_H_
