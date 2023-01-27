#include "PlayMenu.h"

#include <new>
#include <string>
#include <math.h>

#include "Application.h"
#include "PxeFSHelpers.h"
#include "Log.h"
#include "PxeFontManager.h"
#include "PxeEngine.h"
#include "PexGui.h"

using namespace pxengine;
namespace pixelexplorer {
	namespace gui {
		PlayMenu::PlayMenu() {
			_actions = NONE;
			_titleFont = pxeGetEngine().getFontManager().loadFont(getAssetPath("fonts") / "main_menu_title.ttf", 50);
			if (!_titleFont) {
				PEX_ERROR("Failed to create play menu title font");
			}

			_buttonFont = pxeGetEngine().getFontManager().loadFont(getAssetPath("fonts") / "default.ttf", 24);
			if (!_buttonFont) {
				PEX_ERROR("Failed to create play menu button font");
			}

			for (uint8_t i = 0; i < TEXTURE_COUNT; ++i) {
				_textures[i] = new(std::nothrow) PxeTexture();
				if (_textures[i]) {
					_textures[i]->loadImage(getAssetPath("textures") / TEXTURE_FILES[i]);
				}
				else {
					char strBuf[40];
					sprintf_s(strBuf, "Failed to allocate PlayMenu texture %i", i);
					PEX_ERROR(strBuf);
				}
			}
		}

		PlayMenu::~PlayMenu()
		{
			for (uint8_t i = 0; i < TEXTURE_COUNT; ++i) {
				if (_textures[i]) {
					_textures[i]->drop();
					_textures[i] = nullptr;
				}
			}
		}

		uint8_t PlayMenu::getActions()
		{
			uint8_t actions = _actions;
			_actions = NONE;
			return actions;
		}

		void PlayMenu::onRender() {
			ImGuiIO& io = ImGui::GetIO();
			constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Main Menu", nullptr, flags);

			// Background Image
			if (_textures[BACKGROUND]) {
				ImGui::SetCursorPos(ImVec2(0, 0));
				ImGui::Image((ImTextureID)_textures[BACKGROUND]->getGlTextureId(), io.DisplaySize);
			}

			// Title
			{
				if (_titleFont)
					_titleFont->guiPushFont();
				const char* title = "Pixel Explorer";
				ImVec2 textSize = ImGui::CalcTextSize(title);
				ImGui::SetCursorPos(ImVec2((io.DisplaySize.x / 2) - (textSize.x / 2), textSize.y / 2));
				ImGui::Text(title);
				if (_titleFont)
					_titleFont->guiPopFont();
			}

			// Buttons
			{
				if (_buttonFont)
					_buttonFont->guiPushFont();
				ImVec2 buttonSize(4 * ImGui::GetFontSize(), ImGui::GetFontSize());
				ImGui::SetCursorPos(ImVec2((io.DisplaySize.x / 2) - (buttonSize.x / 2), (io.DisplaySize.y / 2) - (buttonSize.y / 2)));
				const char* buttonText = "Play";
				if (_textures[BUTTON_BACKGROUND]) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 1));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 0.75f, 0.75f, 1));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 1, 0.75f, 1));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
					if (PexGui::ImageButton(buttonText, (ImTextureID)_textures[BUTTON_BACKGROUND]->getGlTextureId(), buttonSize))
						_actions |= PLAY;
					ImVec2 textSize = ImGui::CalcTextSize(buttonText);
					ImGui::SetCursorPos(ImVec2((io.DisplaySize.x / 2) - (textSize.x / 2), (io.DisplaySize.y / 2) - (textSize.y / 2)));
					ImGui::Text(buttonText);
					ImGui::PopStyleVar();
					ImGui::PopStyleColor(3);
				}
				else {
					if (ImGui::Button("Play", buttonSize))
						_actions |= PLAY;
				}

				if (_buttonFont)
					_buttonFont->guiPopFont();
			}

			ImGui::End();
			ImGui::PopStyleVar(2);
		}
	}
}