#ifndef PE_GRAPHICS_ADAPTER_H_
#define PE_GRAPHICS_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics.h"
#include "SDL_video.h"

struct PE_DGAPI_GraphicsJumpTable;
#define PE_GRAPHICS_API(rc, fn, params, args, ret) \
	typedef rc (PE_CALL *PE_DGAPI_##fn) params;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API

struct PE_DGAPI_GraphicsJumpTable {
#define PE_GRAPHICS_API(rc, fn, params, args, ret) PE_DGAPI_##fn fn;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
};

// Returns 0 on success and a negative value on error
PE_EXTERN_C PE_NODISCARD int PE_CALL PE_InitGraphicsAdapter();
#endif // !PE_GRAPHICS_ADAPTER_H_
