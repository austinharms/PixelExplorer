#ifndef PE_OPENGL_ADAPTER_H_
#define PE_OPENGL_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics_adapter.h"
#define PE_GRAPHICS_API(rc, fn, params, args, ret)	\
	PE_EXTERN_C rc PE_CALL fn##_OGL params;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
#endif // !PE_OPENGL_ADAPTER_H_
