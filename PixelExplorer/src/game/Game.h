#include <stdint.h>

#include "RefCount.h"
#include "rendering/RenderWindow.h"

#ifndef PIXELEXPLORE_GAME_H_
#define PIXELEXPLORE_GAME_H_
namespace pixelexplore::game {
	class Game : public RefCount
	{
	public:
		Game();
		virtual ~Game();
		void start();

	private:
		rendering::RenderWindow* _renderWindow;
	};
}
#endif // !PIXELEXPLORE_GAME_H_
