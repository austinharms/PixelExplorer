#ifndef PE_GRAPHICS_IMPLEMENTATION_H_
#define PE_GRAPHICS_IMPLEMENTATION_H_
#include "PE_defines.h"
#include "PE_graphics.h"

namespace pe::internal {
	class GraphicsImplementation : public graphics::GraphicsAPI {
	public:
		virtual ~GraphicsImplementation() = default;
		PE_NOCOPY(GraphicsImplementation);

	protected:
		GraphicsImplementation() = default;
	};
}
#endif // !PE_GRAPHICS_IMPLEMENTATION_H_
