#include "ErrorScreen.h"

#include "imgui.h"

namespace pixelexplorer {
	namespace gui {
		void ErrorScreen::setMessage(const char* msg)
		{
			_errorMessage = msg;
		}

		void ErrorScreen::setMessage(const std::string& msg)
		{
			_errorMessage = msg;
		}

		void ErrorScreen::onRender()
		{
			ImGuiIO& io = ImGui::GetIO();
			constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::Begin("ERROR", nullptr, flags);
			ImGui::Text("Pixel Explorer encountered an unrecoverable error");
			ImGui::Text(_errorMessage.c_str());
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}
}