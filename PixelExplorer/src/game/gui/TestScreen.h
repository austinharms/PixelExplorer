#include <stdint.h>
#include <string>

#include "imgui.h"
#include "rendering/GUIElement.h"

#ifndef PIXELEXPLORE_GAME_GUI_TESTSCREEN_H_
#define PIXELEXPLORE_GAME_GUI_TESTSCREEN_H_
namespace pixelexplore::game::gui {
	class TestScreen : public rendering::GUIElement
	{
	public:
		inline TestScreen() : rendering::GUIElement(100) {
			_shouldClose = false;
			_clickCounter = 0;
			_windowFlags = 0;
			_windowFlags |= ImGuiWindowFlags_NoTitleBar;
			_windowFlags |= ImGuiWindowFlags_NoScrollbar;
			_windowFlags |= ImGuiWindowFlags_NoMove;
			_windowFlags |= ImGuiWindowFlags_NoResize;
			_windowFlags |= ImGuiWindowFlags_NoCollapse;
			_windowFlags |= ImGuiWindowFlags_NoNav;
			_windowFlags |= ImGuiWindowFlags_NoBackground;
			_windowFlags |= ImGuiWindowFlags_UnsavedDocument;
			//_windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			//_windowFlags |= ImGuiWindowFlags_MenuBar;
		}

		inline virtual ~TestScreen() {}
		inline void drawElement(float windowWidth, float windowHeight) {
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
			ImGui::Begin("Main Menu", nullptr, _windowFlags);
			ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 25, windowHeight / 2 - 10));
			if (ImGui::Button("Click", ImVec2(50, 20)))
				++_clickCounter;
			ImGui::Text(("Click Count: " + std::to_string(_clickCounter)).c_str());
			if (ImGui::Button("Close", ImVec2(50, 20)))
				_shouldClose = true;
			ImGui::End();
		}

		inline bool getShouldClose() const { return _shouldClose; }

	private:
		uint32_t _clickCounter;
		ImGuiWindowFlags _windowFlags;
		bool _shouldClose;
	};
}
#endif // !PIXELEXPLORE_GAME_GUI_TESTSCREEN_H_
