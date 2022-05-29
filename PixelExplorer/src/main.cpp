#include <stdint.h>
#include "Logger.h"
#include "rendering/RenderWindow.h"

int main(void) {
	pixelexplore::Logger::info("Hello World");
	pixelexplore::rendering::RenderWindow* window = new pixelexplore::rendering::RenderWindow(600, 400, "Pixel Explore");
	while (!window->shouldClose())
	{
		window->drawFrame();
	}

	window->drop();
	return 0;
}