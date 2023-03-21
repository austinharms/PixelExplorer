#include "ErrorScreen.h"

#include <cstring>
#include <malloc.h>

#include "imgui.h"
#include "PxeEngine.h"
#include "PxeRenderPipeline.h"
#include "PxeFSHelpers.h"
#include "Log.h"

namespace pixelexplorer {
	namespace scenes {
		namespace error {
			ErrorScreen::ErrorScreen(pxengine::PxeGuiRenderProperties& renderProperties) : PxeRenderComponent(renderProperties) {
				using namespace pxengine;
				_msg = nullptr;
				_font = PxeEngine::getInstance().getRenderPipeline().loadFont(getAssetPath("fonts") / "default.ttf", 20);
				if (!_font) {
					PEX_FATAL("Failed to create ErrorScreen's PxeFont");
				}
			}

			ErrorScreen::~ErrorScreen() {
				_font->drop();
				if (_msg) free(_msg);
			}

			void ErrorScreen::setMessage(const char* msg)
			{
				if (_msg) free(_msg);
				_msg = static_cast<char*>(malloc(strlen(msg) + 1));
				if (!_msg) {
					PEX_FATAL("Failed to allocate error message buffer");
					return;
				}

				strcpy(_msg, msg);
			}

			void ErrorScreen::onRender()
			{
				ImGuiIO& io = ImGui::GetIO();
				constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
				_font->pushFont();
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
				if (_msg) ImGui::TextWrapped(_msg);
				ImGui::PopTextWrapPos();
				ImGui::PopStyleColor(2);
				ImGui::End();
				ImGui::PopStyleVar();
				_font->popFont();
			}
		}
	}
}