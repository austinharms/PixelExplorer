#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "reactphysics3d/reactphysics3d.h"
#include "rendering/Renderer.h"
#include "rendering/Shader.h"
#define GLM_ENABLE_EXPERIMENTAL

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Pixel Explorer V2.0");

  renderer->setCursorHidden(false);
  
  while (renderer->getWindowOpen()) {
    renderer->drawFrame();
  }

  renderer->drop();
  _CrtDumpMemoryLeaks();
  return 0;
}