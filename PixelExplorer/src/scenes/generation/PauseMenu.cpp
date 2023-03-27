#include "PauseMenu.h"

#include <new>

#include "Log.h"
#include "PxeEngine.h"
#include "PxeRenderPipeline.h"
#include "PxeFSHelpers.h"
#include "common/gui/PexGui.h"

using namespace pxengine;
namespace pixelexplorer {
	namespace scenes {
		namespace generation {
			PauseMenu::PauseMenu() {
				static_assert(TEXTURE_COUNT == 2, "PauseMenu textureFiles array needs to be updated");
				constexpr const char* textureFiles[TEXTURE_COUNT] = { "pause_menu_background.png", "primary_button_background.png" };

				_actions = NONE;
				memset(_textures, 0, sizeof(_textures));
				_menuFont = PxeEngine::getInstance().getRenderPipeline().loadFont(getAssetPath("fonts") / "default.ttf", 14);
				if (!_menuFont) { PEX_FATAL("Failed to load pause menu font"); }
				for (uint8_t i = 0; i < TEXTURE_COUNT; ++i) {
					_textures[i] = new(std::nothrow) PxeTexture();
					if (!_textures[i]) { PEX_FATAL("Failed to allocate PauseMenu PxeTexture"); }
					_textures[i]->loadImage(getAssetPath("textures") / textureFiles[i]);
				}
			}

			PauseMenu::~PauseMenu()
			{
				for (uint8_t i = 0; i < TEXTURE_COUNT; ++i) {
					if (_textures[i])
						_textures[i]->drop();
				}

				if (_menuFont)
					_menuFont->drop();
			}

			uint8_t PauseMenu::getActions()
			{
				uint8_t actions = _actions;
				_actions = NONE;
				return actions;
			}

			void PauseMenu::setEnabled(bool enabled)
			{
				_enabled = enabled;
			}

			bool PauseMenu::getEnabled() const
			{
				return _enabled;
			}

			void PauseMenu::onRender() {
				if (!_enabled) return;
				ImGuiIO& io = ImGui::GetIO();
				constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
				ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
				ImGui::SetNextWindowSize(io.DisplaySize);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.25f));
				ImGui::Begin("Pause Menu", nullptr, flags);
				_menuFont->pushFont();
				ImVec2 menuSize(ImGui::GetFontSize() * 10, ImGui::GetFontSize() * 25);
				ImVec2 menuPos(io.DisplaySize.x / 2 - menuSize.x / 2, io.DisplaySize.y / 2 - menuSize.y / 2);

				// Background Image
				if (_textures[BACKGROUND]->getTextureLoaded()) {
					ImGui::SetCursorPos(menuPos);
					ImGui::Image((ImTextureID)_textures[BACKGROUND]->getGlTextureId(), menuSize);
				}
				else {
					ImGui::GetBackgroundDrawList()->AddRectFilled(menuPos, ImVec2(menuPos.x + menuSize.x, menuPos.y + menuSize.y), IM_COL32(0, 195, 255, 255));
				}

				// Title
				{
					const char* title = "Paused";
					ImVec2 textSize = ImGui::CalcTextSize(title);
					ImGui::SetCursorPos(ImVec2(menuPos.x + (menuSize.x / 2 - textSize.x / 2), menuPos.y + textSize.y));
					ImGui::Text(title);
				}

				// Buttons
				{
					ImVec2 buttonSize(4 * ImGui::GetFontSize(), ImGui::GetFontSize());
					ImGui::SetCursorPos(ImVec2((io.DisplaySize.x / 2) - (buttonSize.x / 2), (io.DisplaySize.y / 2) - (buttonSize.y / 2)));
					const char* buttonText = "Play";
					if (_textures[BUTTON_BACKGROUND]->getTextureLoaded()) {
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
						// TODO Update fallback button
						if (ImGui::Button(buttonText, buttonSize))
							_actions |= PLAY;
					}
				}

				_menuFont->popFont();
				ImGui::End();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);
			}
		}
	}
}