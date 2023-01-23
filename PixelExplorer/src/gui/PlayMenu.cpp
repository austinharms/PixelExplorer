#include "PlayMenu.h"

#include <new>

#include "PxeFSHelpers.h"
#include "imgui.h"
#include "Log.h"

namespace pixelexplorer {
	namespace gui {
		PlayMenu::PlayMenu() {
			using namespace pxengine;
			memset(_textures, 0, sizeof(_textures));
			_textures[0] = new(std::nothrow) PxeTexture();
			if (_textures[0]) {
				_textures[0]->loadImage(getAssetPath("textures") / "main_menu_background.png");
			}
			else {
				PEX_ERROR("Failed to allocate PlayMenu background texture");
			}
		}

		PlayMenu::~PlayMenu()
		{
			if (_textures[0])
				_textures[0]->drop();
		}

		void PlayMenu::onRender() {
			ImGuiIO& io = ImGui::GetIO();
			constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_UnsavedDocument;
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(io.DisplaySize);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::Begin("Main Menu", nullptr, flags);
			
			// Background Image
			if (_textures[0]) {
				ImGui::SetCursorPos(ImVec2(0, 0));
				ImGui::Image((ImTextureID)_textures[0]->getGlTextureId(), io.DisplaySize);
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}
}