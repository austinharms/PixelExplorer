#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdint.h>
#include "Logger.h"
#include "rendering/RenderWindow.h"
#include "rendering/RenderMesh.h"

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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