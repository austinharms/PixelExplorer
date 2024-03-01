#ifndef PE_GRAPHICS_ADAPTER_H_
#define PE_GRAPHICS_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics_types.h"

namespace pecore::pe_graphics {
	class GraphicsAdapterInterface {
	public:
		// Safe to assume this will be called on the event aka main thread
		// Returns values should be stored in ret_val
		// Positive return values are reserved for PE_ERROR_* codes
		GraphicsAdapterInterface(int* ret_val) {}

		// Safe to assume this will be called on the event aka main thread
		virtual ~GraphicsAdapterInterface() = default;

#define PE_GRAPHICS_API(rc, fn, params, args, ret) virtual rc fn params = 0;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
	};
}
#endif // !PE_GRAPHICS_ADAPTER_H_
