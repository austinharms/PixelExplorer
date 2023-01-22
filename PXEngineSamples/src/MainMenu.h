#ifndef PXENGINESAMPELS_MAIN_MENU_H_
#define PXENGINESAMPELS_MAIN_MENU_H_
#include "PxeEngineAPI.h"
#include "imgui.h"

class MainMenu : public pxengine::PxeRenderElement
{
public:
	enum MENUACTIONS
	{
		NONE = 0,
		QUIT = 0x1,
		CUBEWALL = 0x2,
		CUBESTACK = 0x4,
		MARCHINGCUBES = 0x8,
		ALL = 0xff
	};

	MainMenu()
	{
		_clickedActions = NONE;
	}

	virtual ~MainMenu() {}

	void onRender() override
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
		ImGui::Text("Main Menu");
		ImGui::Text("Open Sample Scene:");
		ImGui::Bullet();
		if (ImGui::SmallButton("Cube Wall"))
			_clickedActions |= CUBEWALL;
		ImGui::Bullet();
		if (ImGui::SmallButton("Cube Stack"))
			_clickedActions |= CUBESTACK;
		ImGui::Bullet();
		if (ImGui::SmallButton("Marching Cubes"))
			_clickedActions |= MARCHINGCUBES;
		if (ImGui::Button("Quit"))
			_clickedActions |= QUIT;
		ImGui::End();
		ImGui::PopStyleVar();
	}

	uint8_t getClickedActions()
	{
		uint8_t lastActions = _clickedActions;
		_clickedActions = NONE;
		return lastActions;
	}

private:
	uint8_t _clickedActions;
};
#endif // !PXENGINESAMPELS_MAIN_MENU_H_
