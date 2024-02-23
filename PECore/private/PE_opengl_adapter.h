#ifndef PE_DISABLE_OPENGL
#ifndef PE_OPENGL_ADAPTER_H_
#define PE_OPENGL_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics_adapter.h"

namespace pecore::pe_graphics::open_gl {
	class OpenGLGraphicsAdapter : public GraphicsAdapterInterface {
	public:
		OpenGLGraphicsAdapter(int* ret_val);
		virtual ~OpenGLGraphicsAdapter();
#define PE_GRAPHICS_API(rc, fn, params, args, ret) rc fn params PE_OVERRIDE;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
	};
}
#endif // !PE_OPENGL_ADAPTER_H_
#endif // !PE_DISABLE_OPENGL
