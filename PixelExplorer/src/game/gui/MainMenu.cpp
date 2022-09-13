#include "MainMenu.h"

#include <string>
#include <math.h>
#include <filesystem>

#include "common/Logger.h"
#include "common/OSHelpers.h"
#include "engine/rendering/RenderWindow.h"

namespace pixelexplorer::game::gui {
	MainMenu::MainMenu() {
		_currentAction = MainMenuAction::NONE;
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
		_backgroundTexture = nullptr;
		_font = nullptr;
	}

	MainMenu::~MainMenu() {}

	void MainMenu::onInitialize() {
		_font = getRenderWindow()->getFont( OSHelper::getAssetPath(std::filesystem::path("fonts")) / "roboto.ttf");
		_backgroundTexture = new engine::rendering::GLTexture(OSHelper::getAssetPath(std::filesystem::path("textures")) / "main_menu_background.png");
		getRenderWindow()->registerGLObject(_backgroundTexture);
	}

	void MainMenu::onTerminate() {
		// should font be a GLObject? how is font memory managed?
		_font = nullptr;
		_backgroundTexture->drop();
		_backgroundTexture = nullptr;
	}
	void MainMenu::startWindow(float windowWidth, float windowHeight, float uiScale)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
		ImGui::Begin("Main Menu", nullptr, _windowFlags);
	}

	void MainMenu::endWindow()
	{
		ImGui::End();
	}

	void MainMenu::startStyles(float windowWidth, float windowHeight, float uiScale)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);
		ImGui::PushStyleColor(ImGuiCol_Button, 0xff737373);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff969696);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xff4a4a4a);
		ImGui::PushFont(_font);
	}

	void MainMenu::endStyles()
	{
		ImGui::PopStyleColor(4);
		ImGui::PopFont();
	}

	void MainMenu::drawTitle(float windowWidth, float windowHeight, float uiScale)
	{
		ImGui::SetWindowFontScale(2 * uiScale);
		ImVec2 textSize = ImGui::CalcTextSize("Pixel Explorer");
		ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2), 50 * uiScale));
		ImGui::Text("Pixel Explorer");
	}

	void MainMenu::drawPlayButton(float windowWidth, float windowHeight, float uiScale)
	{
		ImGui::SetWindowFontScale(1 * uiScale);
		ImVec2 textSize = ImGui::CalcTextSize("Play");
		ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2) - (10 * uiScale), (windowHeight / 2) - (textSize.y / 2) + (5 * uiScale)));
		if (ImGui::Button("Play", ImVec2(textSize.x + (20 * uiScale), textSize.y + (10 * uiScale))))
			_currentAction = MainMenuAction::PLAY;
	}

	void MainMenu::drawCloseButton(float windowWidth, float windowHeight, float uiScale)
	{
		ImGui::SetWindowFontScale(1 * uiScale);
		ImVec2 textSize = ImGui::CalcTextSize("Play");
		ImGui::SetCursorPos(ImVec2((windowWidth / 2) - (textSize.x / 2) - (10 * uiScale), (windowHeight / 2) - (textSize.y / 2) + (5 * uiScale) + textSize.y + (10 * uiScale)));
		if (ImGui::Button("Close", ImVec2(textSize.x + (20 * uiScale), textSize.y + (10 * uiScale))))
			_currentAction = MainMenuAction::CLOSE;
	}

	void MainMenu::drawBackground(float windowWidth, float windowHeight, float uiScale)
	{
		// Draw Background Image (scaled to fill screen)
		float imageScale = fmaxf(windowWidth / _backgroundTexture->getWidth(), windowHeight / _backgroundTexture->getHeight());
		ImGui::SetCursorPos(ImVec2(-(((_backgroundTexture->getWidth() * imageScale) - windowWidth) / 2), -(((_backgroundTexture->getHeight() * imageScale) - windowHeight) / 2)));
		ImGui::Image((ImTextureID)_backgroundTexture->getGlId(), ImVec2(_backgroundTexture->getWidth() * imageScale, _backgroundTexture->getHeight() * imageScale));
	}

	void MainMenu::onRenderGui(float windowWidth, float windowHeight, float uiScale)
	{
		startWindow(windowWidth, windowHeight, uiScale);
		startStyles(windowWidth, windowHeight, uiScale);
		drawBackground(windowWidth, windowHeight, uiScale);
		drawTitle(windowWidth, windowHeight, uiScale);
		drawPlayButton(windowWidth, windowHeight, uiScale);
		drawCloseButton(windowWidth, windowHeight, uiScale);
		endStyles();
		endWindow();
	}
}