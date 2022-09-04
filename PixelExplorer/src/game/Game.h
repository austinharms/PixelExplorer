#include <stdint.h>

#include "common/RefCount.h"

#ifndef PIXELEXPLORE_GAME_GAME_H_
#define PIXELEXPLORE_GAME_GAME_H_
namespace pixelexplorer::game {
	class Game : public RefCount
	{
	public:
		Game();
		virtual ~Game();
		void start();

	private:
	};
}
#endif // !PIXELEXPLORE_GAME_H_
