#include <stdint.h>
#include <string>

#include "Logger.h"
#include "imgui.h"
#include "rendering/RenderWindow.h"
#include "rendering/GUIElement.h"
#include "rendering/GLTexture.h"

#ifndef PIXELEXPLORE_GAME_GUI_TESTSCREEN_H_
#define PIXELEXPLORE_GAME_GUI_TESTSCREEN_H_
namespace pixelexplorer::game::gui {
	class TestScreen : public rendering::GUIElement
	{
	public:
		inline TestScreen() {
			_shouldClose = false;
			_removeTestMesh = false;
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
			_texture = new rendering::GLTexture("./assets/textures/testbackground.png");
		}

		inline virtual ~TestScreen() { _texture->drop(); }

		inline void onInitialize() override {
			if (_texture->getRenderWindow() == nullptr)
				getRenderWindow()->addGLAsset(_texture);
		}

		inline void onRenderGui(float windowWidth, float windowHeight, float uiScale) override {
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
			ImGui::Begin("Main Menu", nullptr, _windowFlags);
			ImGui::SetCursorPos(ImVec2(0, 0));
			ImGui::Image((ImTextureID)_texture->getGlId(), ImVec2(windowWidth, windowHeight));
			ImGui::SetCursorPos(ImVec2(windowWidth / 2 - 25, windowHeight / 2 - 10));
			if (ImGui::Button("Click", ImVec2(50, 20)))
				++_clickCounter;
			ImGui::Text(("Click Count: " + std::to_string(_clickCounter)).c_str());
			if (ImGui::Button("Close", ImVec2(50, 20)))
				_shouldClose = true;
			if (ImGui::Button("Remove", ImVec2(50, 20)))
				_removeTestMesh = true;
			ImGui::End();
		}

		inline bool getShouldClose() const { return _shouldClose; }
		inline bool getRemoveTestMesh() const { return _removeTestMesh; }
		inline bool requiresGLObjects() { return true; }

	private:
		rendering::GLTexture* _texture;
		uint32_t _clickCounter;
		ImGuiWindowFlags _windowFlags;
		bool _shouldClose;
		bool _removeTestMesh;
	};
}
#endif // !PIXELEXPLORE_GAME_GUI_TESTSCREEN_H_
