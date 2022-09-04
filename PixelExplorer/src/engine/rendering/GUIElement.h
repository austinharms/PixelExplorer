#include <stdint.h>

#include "GLRenderObject.h"
#include "RenderWindow.h"

#ifndef PIXELEXPLORE_ENGINE_RENDERING_GUIELEMENT_H_
#define PIXELEXPLORE_ENGINE_RENDERING_GUIELEMENT_H_
namespace pixelexplorer::engine::rendering {
	class GUIElement : public GLRenderObject
	{
	public:
		inline GUIElement() {
			// render after onUpdate
			setPriority(1);
		}

		inline virtual ~GUIElement() {}

		virtual void onRenderGui(float windowWidth, float windowHeight, float uiScale) = 0;

	private:
		inline void onUpdate() override {
			RenderWindow* window = getRenderWindow();
			window->loadImGuiContext();
			onRenderGui(window->getWindowWidth(), window->getWindowHeight(), window->getWindowScale());
		}
	};
}
#endif // !PIXELEXPLORE_RENDERING_GUIELEMENT_H_
