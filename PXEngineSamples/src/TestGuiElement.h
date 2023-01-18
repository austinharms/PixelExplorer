#ifndef PXENGINESAMPELS_TEST_GUI_ELEMENT_H_
#define PXENGINESAMPELS_TEST_GUI_ELEMENT_H_
#include "PxeEngineAPI.h"
#include "imgui.h"

class TestGuiElement : public pxengine::PxeRenderElement
{
public:
	TestGuiElement()
	{
		_clicked = false;
	}

	virtual ~TestGuiElement() {}

	void onRender() override 
	{
		ImGui::ShowDemoWindow();
		_clicked = ImGui::Button("Button");
	}

	bool getClicked() const { return _clicked; }

private:
	bool _clicked;
};
#endif // !PXENGINESAMPELS_TEST_GUI_ELEMENT_H_
