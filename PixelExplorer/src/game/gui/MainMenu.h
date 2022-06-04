#include <stdint.h>
#include <string>
#include <math.h>

#include "Logger.h"
#include "imgui.h"
#include "rendering/RenderWindow.h"
#include "rendering/GUIElement.h"
#include "rendering/Texture.h"

#ifndef PIXELEXPLORE_GAME_GUI_MAINMENU_H_
#define PIXELEXPLORE_GAME_GUI_MAINMENU_H_
namespace pixelexplorer::game::gui {
	class MainMenu : public rendering::GUIElement
	{
	public:
		inline MainMenu() : rendering::GUIElement(100) {
			_shouldClose = false;
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
			_backgroundTexture = new rendering::Texture("./assets/textures/main_menu_background.png");
			_font = nullptr;
		}

		inline virtual ~MainMenu() { _backgroundTexture->drop(); }

		inline void drawElement(float windowWidth, float windowHeight, float uiScale) {

			// Create Full Screen window and set styles
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
			ImGui::Begin("Main Menu", nullptr, _windowFlags);
			ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
			ImGui::PushStyleColor(ImGuiCol_Button, 0xff737373);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff969696);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xff4a4a4a);
			ImGui::PushFont(_font);

			// Draw Background Image (scaled to fill screen)
			float imageScale = fmaxf(windowWidth / _backgroundTexture->getWidth(), windowHeight / _backgroundTexture->getHeight());
			ImGui::SetCursorPos(ImVec2(-(((_backgroundTexture->getWidth() * imageScale) - windowWidth) / 2), -(((_backgroundTexture->getHeight() * imageScale) - windowHeight) / 2)));
			ImGui::Image((ImTextureID)_backgroundTexture->getGlId(), ImVec2(_backgroundTexture->getWidth() * imageScale, _backgroundTexture->getHeight() * imageScale));

			// Draw Title
			ImGui::SetWindowFontScale(roundf(2 * uiScale));
			ImVec2 textSize = ImGui::CalcTextSize("Pixel Explorer");
			ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2), 50 * uiScale));
			ImGui::Text("Pixel Explorer");

			// Draw Play/Close Button
			ImGui::SetWindowFontScale(roundf(1 * uiScale));
			textSize = ImGui::CalcTextSize("Play");
			ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2) - (10 * uiScale), (windowHeight / 2) - (textSize.y / 2) + (5 * uiScale)));
			if (ImGui::Button("Play", ImVec2(textSize.x + (20 * uiScale), textSize.y + (10 * uiScale))))
				_shouldClose = true;

			ImGui::PopStyleColor(4);
			ImGui::PopFont();
			ImGui::End();

		}

		inline void createGLObjects(rendering::RenderWindow* window) {
			if (getHasGLObjects()) {
				Logger::warn(__FUNCTION__ " Attempted to overwrite GL objects");
				return;
			}

			_font = window->loadFont("./assets/fonts/roboto.ttf");
			_backgroundTexture->createGlTexture();
			setHasGLObjects(true);
		}

		inline void destroyGLObjects(rendering::RenderWindow* window) {
			if (!getHasGLObjects()) {
				Logger::warn(__FUNCTION__ " Attempted to delete empty GL objects");
				return;
			}

			// Font memory is handled by ImGui
			_font = nullptr;

			_backgroundTexture->deleteGlTexture();
			setHasGLObjects(false);
		}

		inline bool getShouldClose() const { return _shouldClose; }

		inline bool requiresGLObjects() { return true; }

	private:
		rendering::Texture* _backgroundTexture;
		ImFont* _font;
		ImGuiWindowFlags _windowFlags;
		bool _shouldClose;
	};
}
#endif // !PIXELEXPLORE_GAME_GUI_MAINMENU_H_
