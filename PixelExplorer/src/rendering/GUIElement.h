#include <stdint.h>

#include "RefCount.h"

#ifndef PIXELEXPLORE_RENDERING_GUIELEMENT_H_
#define PIXELEXPLORE_RENDERING_GUIELEMENT_H_
namespace pixelexplore::rendering {
	class GUIElement : public RefCount
	{
	public:
		inline GUIElement(uint32_t zIndex = 0) { this->zIndex = zIndex; }
		inline virtual ~GUIElement() {}
		virtual uint32_t getZIndex() { return zIndex; }
		virtual void drawElement(float windowWidth, float windowHeight) = 0;
	protected:
		// 0 is first layer and is behide all other layers
		uint32_t zIndex;
	};
}
#endif // !PIXELEXPLORE_RENDERING_GUIELEMENT_H_
