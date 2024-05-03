#ifndef PE_GRAPHICS_IMPLEMENTATION_H_
#define PE_GRAPHICS_IMPLEMENTATION_H_
#include "PE_defines.h"
#include "PE_graphics.h"

namespace pecore::graphics {
#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret) typedef rc (*fn##_GDAPI) params;
#include "PE_generated_graphics_api.h"
#undef PE_GENERATED_GRAPHICS_API

	struct GraphicsCommands {
	public:
#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret) fn##_GDAPI fn;
#include "PE_generated_graphics_api.h"
#undef PE_GENERATED_GRAPHICS_API
	};
}
#endif // !PE_GRAPHICS_IMPLEMENTATION_H_
