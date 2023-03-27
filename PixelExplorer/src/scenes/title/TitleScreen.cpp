#include "TitleScreen.h"

#include <new>

#include "Application.h"
#include "PxeEngine.h"
#include "PxeRenderPipeline.h"
#include "PxeFSHelpers.h"
#include "Log.h"
#include "imgui.h"
#include "common/gui/PexGui.h"

namespace pixelexplorer {
	namespace scenes {
		namespace title {
			TitleScreen::TitleScreen() {
				using namespace pxengine;
				static_assert(TEXTURE_COUNT == 2, "TitleScreen textureFiles array needs to be updated");
				constexpr const char* textureFiles[TEXTURE_COUNT] = { "main_menu_background.png", "primary_button_background.png" };
				static_assert(FONT_COUNT == 2, "TitleScreen fontFiles array needs to be updated");
				constexpr const uint16_t fontSizes[FONT_COUNT] = { 50, 24 };
				constexpr const char* fontFiles[FONT_COUNT] = { "main_menu_title.ttf", "default.ttf" };

				memset(_textures, 0, sizeof(_textures));
				memset(_fonts, 0, sizeof(_textures));
				_actions = NONE;
				PxeRenderPipeline& pipe = PxeEngine::getInstance().getRenderPipeline();
				for (uint8_t i = 0; i < FONT_COUNT; ++i) {
					_fonts[i] = pipe.loadFont(getAssetPath("fonts") / fontFiles[i], fontSizes[i]);
					if (!_fonts[i]) {
						PEX_FATAL("Failed to create TitleScreen's PxeFont");
						return;
					}
				}

				for (uint8_t i = 0; i < TEXTURE_COUNT; ++i) {
					_textures[i] = new(std::nothrow) PxeTexture();
					if (!_textures[i]) {
						PEX_FATAL("Failed to allocate TitleScreen's PxeTexture");
						return;
					}

					_textures[i]->loadImage(getAssetPath("textures") / textureFiles[i]);
				}
			}

			TitleScreen::~TitleScreen()
			{
				for (uint8_t i = 0; i < FONT_COUNT; ++i) {
					if (_fonts[i]) _fonts[i]->drop();
				}

				for (uint8_t i = 0; i < TEXTURE_COUNT; ++i) {
					if (_textures[i]) _textures[i]->drop();
				}
			}

			TitleScreen::TitleScreenAction TitleScreen::getActions()
			{
				TitleScreenAction act = _actions;
				_actions = NONE;
				return act;
			}

			TitleScreen::TitleScreenAction TitleScreen::peekActions() const
			{
				return _actions;
			}

			void TitleScreen::clearActions()
			{
				_actions = NONE;
			}

			void TitleScreen::onRender()
			{
				ImGuiIO& io = ImGui::GetIO();
				constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
				ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
				ImGui::SetNextWindowSize(io.DisplaySize);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				ImGui::Begin("Main Menu", nullptr, flags);

				// Background Image
				if (_textures[BACKGROUND]->getTextureLoaded()) {
					ImGui::SetCursorPos(ImVec2(0, 0));
					ImGui::Image((ImTextureID)_textures[BACKGROUND]->getGlTextureId(), io.DisplaySize);
				}
				else {
					ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), io.DisplaySize, IM_COL32(0, 195, 255, 255));
				}

				// Title
				{
					_fonts[TITLE]->pushFont();
					const char* title = "Pixel Explorer";
					ImVec2 textSize = ImGui::CalcTextSize(title);
					ImGui::SetCursorPos(ImVec2((io.DisplaySize.x / 2) - (textSize.x / 2), textSize.y / 2));
					ImGui::Text(title);
					_fonts[TITLE]->popFont();
				}

				// Buttons
				{
					_fonts[BUTTON]->pushFont();
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
						// TODO Update fall back button
						if (ImGui::Button(buttonText, buttonSize))
							_actions |= PLAY;
					}

					_fonts[BUTTON]->popFont();
				}

				ImGui::End();
				ImGui::PopStyleVar(2);
			}
		}
	}
}