#ifndef PXENGINESAMPELS_MAIN_MENU_H_
#define PXENGINESAMPELS_MAIN_MENU_H_
#include "PxeEngineAPI.h"
#include "imgui.h"

class MainMenu : public pxengine::PxeRenderComponent
{
public:
	enum MENUACTIONS
	{
		NONE = 0,
		QUIT = 0x1,
		CUBEWALL = 0x2,
		CUBESTACK = 0x4,
		FREECAMERA = 0x10,
		ALL = 0xff
	};

	static pxengine::PxeObject& createMainMenuObject() {
		using namespace pxengine;
		PxeObject& obj = *PxeObject::create();
		PxeGuiRenderProperties* guiProperties = PxeGuiRenderProperties::getInstance();
		MainMenu& menu = *(new MainMenu(*guiProperties));
		guiProperties->drop();
		obj.addComponent(menu);
		menu.drop();
		return obj;
	}

	virtual ~MainMenu() {}

	void onRender() override
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::Text("FPS: %.0f", io.Framerate);
		ImGui::Text("Main Menu");
		ImGui::Text("Open Sample Scene:");
		ImGui::Bullet();
		if (ImGui::SmallButton("Cube Wall"))
			_clickedActions |= CUBEWALL;
		ImGui::Bullet();
		if (ImGui::SmallButton("Cube Stack"))
			_clickedActions |= CUBESTACK;
		ImGui::Bullet();
		if (ImGui::SmallButton("Free Camera"))
			_clickedActions |= FREECAMERA;
		if (ImGui::Button("Quit"))
			_clickedActions |= QUIT;
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::ShowDemoWindow();
	}

	uint8_t getClickedActions()
	{
		uint8_t lastActions = _clickedActions;
		_clickedActions = NONE;
		return lastActions;
	}

	PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(MainMenu, pxengine::PxeRenderComponent);

protected:
	PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(pxengine::PxeRenderComponent);

private:
	MainMenu(pxengine::PxeRenderProperties& renderProps) : pxengine::PxeRenderComponent(renderProps)
	{
		_clickedActions = NONE;
	}

	uint8_t _clickedActions;
};
#endif // !PXENGINESAMPELS_MAIN_MENU_H_
