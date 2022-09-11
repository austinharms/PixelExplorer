#include <stdint.h>

#include "imgui.h"
#include "engine/rendering/GUIElement.h"
#include "engine/rendering/GLTexture.h"

#ifndef PIXELEXPLORE_GAME_GUI_MAINMENU_H_
#define PIXELEXPLORE_GAME_GUI_MAINMENU_H_
namespace pixelexplorer::game::gui {
	class MainMenu : public engine::rendering::GUIElement
	{
	public:
		enum class MainMenuAction : uint8_t
		{
			NONE = 0,
			PLAY,
			CLOSE
		};

		MainMenu();
		virtual ~MainMenu();
		void onInitialize() override;
		void onTerminate() override;
		void onRenderGui(float windowWidth, float windowHeight, float uiScale) override;

		inline MainMenuAction getMenuAction() const { return _currentAction; }

	private:
		engine::rendering::GLTexture* _backgroundTexture;
		ImFont* _font;
		ImGuiWindowFlags _windowFlags;
		MainMenuAction _currentAction;

		void startWindow(float windowWidth, float windowHeight, float uiScale);
		void endWindow();
		void startStyles(float windowWidth, float windowHeight, float uiScale);
		void endStyles();
		void drawTitle(float windowWidth, float windowHeight, float uiScale);
		void drawPlayButton(float windowWidth, float windowHeight, float uiScale);
		void drawCloseButton(float windowWidth, float windowHeight, float uiScale);
		void drawBackground(float windowWidth, float windowHeight, float uiScale);
	};
}
#endif // !PIXELEXPLORE_GAME_GUI_MAINMENU_H_
