#include "ErrorScreen.h"

#include "imgui.h"
#include "PxeEngine.h"
#include "PxeFontManager.h"
#include "PxeFSHelpers.h"
#include "Log.h"

namespace pixelexplorer {
	namespace gui {
		void ErrorScreen::setMessage(const char* msg)
		{
			_errorMessage = msg;
			_font = pxengine::pxeGetEngine().getFontManager().loadFont(pxengine::getAssetPath("fonts") / "default.ttf", 20);
			if (!_font) {
				PEX_ERROR("Failed to load error font");
			}
		}

		void ErrorScreen::setMessage(const std::string& msg)
		{
			_errorMessage = msg;
		}

		void ErrorScreen::onGUI()
		{
			ImGuiIO& io = ImGui::GetIO();
			constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
			if (_font) {
				_font->guiPushFont();
			}
			else {
				ImGui::PushFont(nullptr);
				io.FontGlobalScale = fminf(
					floorf(20.0f * (ImGui::GetIO().DisplaySize.x / 600.0f)),
					floorf(20.0f * (ImGui::GetIO().DisplaySize.y / 400.0f))
				) / 13.0f;
			}

			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0.75f, 1, 1));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0, 0, 1));
			ImGui::Begin("ERROR", nullptr, flags);
			const char* title = "Pixel Explorer encountered an unrecoverable error";
			ImVec2 titleSize = ImGui::CalcTextSize(title);
			ImGui::SetCursorPos(ImVec2(io.DisplaySize.x / 2 - titleSize.x / 2, titleSize.y / 2));
			ImGui::Text(title);
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
			ImGui::SetCursorPos(ImVec2(ImGui::GetFontSize() * 4, titleSize.y * 2));
			io.FontGlobalScale /= 2.0f;
			// Hack to force FontGlobalScale to apply
			ImGui::PushFont(nullptr);
			ImGui::PopFont();
			ImGui::PushTextWrapPos(io.DisplaySize.x - ImGui::GetFontSize() * 4);
			ImGui::TextWrapped(_errorMessage.c_str());
			ImGui::PopTextWrapPos();
			ImGui::PopStyleColor(2);
			ImGui::End();
			ImGui::PopStyleVar();
			if (_font) {
				_font->guiPopFont();
			}
			else {
				ImGui::PopFont();
			}
		}
	}
}