#include "PE_graphics.h"
#include "PE_graphics_adapter.h"
#include "PE_opengl_adapter.h"
#include "PE_log.h"

PE_EXTERN_C static PE_DGAPI_GraphicsJumpTable l_graphicsJumpTable;
#define PE_GRAPHICS_API(rc, fn, params, args, ret) \
    PE_EXTERN_C rc PE_CALL fn params        \
    {                                              \
        ret l_graphicsJumpTable.fn args;           \
    }
#include "PE_graphics_api.h"
#undef PE_GRAPHICS_API

PE_EXTERN_C PE_NODISCARD int PE_CALL PE_InitGraphicsAdapter() {
    // TODO Replace this with a dynamic selection from all available adapters
    int res = PE_InitGraphics_OGL(&l_graphicsJumpTable);
#define PE_GRAPHICS_API(rc, fn, params, args, ret) \
    PE_ASSERT(l_graphicsJumpTable.##fn != nullptr, PE_TEXT("PE_graphics failed load function #fn"));
#include "PE_graphics_api.h"
#undef PE_GRAPHICS_API
    return res;
}
