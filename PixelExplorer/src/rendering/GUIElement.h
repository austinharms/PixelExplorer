#include <stdint.h>

#include "BasicGLRenderObject.h"
#include "RenderWindow.h"

#ifndef PIXELEXPLORE_RENDERING_GUIELEMENT_H_
#define PIXELEXPLORE_RENDERING_GUIELEMENT_H_
namespace pixelexplorer::rendering {
	class GUIElement : public BasicGLRenderObject
	{
	public:
		inline GUIElement() {}
		inline virtual ~GUIElement() {}
		virtual void onRenderGui(float windowWidth, float windowHeight, float uiScale) = 0;

	private:
		inline void onRender() override {
			RenderWindow* window = getRenderWindow();
			window->loadImGuiContext();
			onRenderGui(window->getWindowWidth(), window->getWindowHeight(), window->getWindowScale());
		}
	};
}
#endif // !PIXELEXPLORE_RENDERING_GUIELEMENT_H_
