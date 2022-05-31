#include <stdint.h>
#include "Logger.h"
#include "rendering/RenderWindow.h"
#include "rendering/RenderMesh.h"

int main(void) {
	pixelexplore::rendering::RenderWindow* window = new pixelexplore::rendering::RenderWindow(600, 400, "Pixel Explore");
	pixelexplore::rendering::RenderMesh* renderMesh = new pixelexplore::rendering::RenderMesh();
	window->addRenderMesh(renderMesh);
	while (!window->shouldClose())
	{
		window->drawFrame();
	}

	window->removeRenderMesh(renderMesh);
	renderMesh->drop();
	window->drop();
	return 0;
}