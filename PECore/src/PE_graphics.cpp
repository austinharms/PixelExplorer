#include "PE_graphics.h"
#include "PE_graphics_adapter.h"
#include "PE_opengl_adapter.h"
#include "PE_log.h"

static PE_DGAPI_GraphicsJumpTable l_graphicsJumpTable;

#define PE_GRAPHICS_API(rc, fn, params, args, ret) \
    rc fn params        \
    {                                              \
        ret l_graphicsJumpTable.fn args;           \
    }
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API

int PE_InitGraphicsAdapter() {
    // TODO Replace this with a dynamic selection from all available adapters
    int res = PE_InitGraphics_OGL(&l_graphicsJumpTable);
    if (res != 0) {
        memset(&l_graphicsJumpTable, 0, sizeof(PE_DGAPI_GraphicsJumpTable));
        return res;
    }

#define PE_GRAPHICS_API(rc, fn, params, args, ret) \
    PE_ASSERT(l_graphicsJumpTable.##fn != nullptr, PE_TEXT("PE_graphics failed load function ") PE_TEXT(#fn));
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
    return res;
}
