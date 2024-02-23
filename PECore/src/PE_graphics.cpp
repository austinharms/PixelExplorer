#include "PE_graphics.h"
#include "PE_graphics_adapter.h"
#include "PE_opengl_adapter.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_log.h"
#include <new>

namespace pecore::pe_graphics {
    static GraphicsAdapterInterface* s_graphics_adapter = nullptr;

#define PE_GRAPHICS_API(rc, fn, params, args, ret)  \
    rc PE_##fn params {                             \
        ret s_graphics_adapter->fn args;              \
    }
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API

    int PE_InitGraphicsAdapter() {
        if (s_graphics_adapter) {
            return PE_ERROR_ALREADY_INITIALIZED;
        }

        // TODO Replace this with a dynamic selection from all available adapters
        void* adapter_mem = PE_malloc(sizeof(open_gl::OpenGLGraphicsAdapter));
        if (!adapter_mem) {
            PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate OpenGLGraphicsAdapter"));
            return PE_ERROR_OUT_OF_MEMORY;
        }

        int res;
        s_graphics_adapter = new(adapter_mem) open_gl::OpenGLGraphicsAdapter(&res);
        if (res != PE_ERROR_NONE) {
            s_graphics_adapter->~GraphicsAdapterInterface();
            s_graphics_adapter = nullptr;
            PE_free(adapter_mem);
        }

        return res;
    }
}
