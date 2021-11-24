#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "World.h"
#include "chunk/Chunk.h"
#include "chunk/block/BaseBlock.h"
#include "reactphysics3d/reactphysics3d.h"
#include "rendering/Renderer.h"
#include "rendering/Shader.h"
#include "rendering/TestRenderable.h"

int main(void) {
  Renderer* renderer = new Renderer(1200, 800, "Pixel Explorer V2.0");
  renderer->setCursorHidden(false);
  World::SetRenderer(renderer);
  World::LoadWorld();
  World::GetChunkManager()->LoadChunk(glm::vec<3, int32_t>(0, 0, 0));
  glm::vec3 camPos(12, 12, 75);
  glm::vec3 camRot(0,0,0);
  float moveSpeed = 5;

  while (renderer->getWindowOpen()) {
    if (renderer->getKeyPressed(GLFW_KEY_W)) {
      camPos += renderer->getDeltaTime() * renderer->getForward() * moveSpeed;
    } else if (renderer->getKeyPressed(GLFW_KEY_S)) {
      camPos -= renderer->getDeltaTime() * renderer->getForward() * moveSpeed;
    }

    if (renderer->getKeyPressed(GLFW_KEY_A)) {
      camPos -= renderer->getDeltaTime() * glm::cross(renderer->getForward(), glm::vec3(0,1,0)) * moveSpeed;
    } else if (renderer->getKeyPressed(GLFW_KEY_D)) {
      camPos += renderer->getDeltaTime() * glm::cross(renderer->getForward(), glm::vec3(0,1,0)) * moveSpeed;
    }

    if (renderer->getKeyPressed(GLFW_KEY_SPACE)) {
      camPos.y += renderer->getDeltaTime() * moveSpeed;
    } else if (renderer->getKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
      camPos.y -= renderer->getDeltaTime() * moveSpeed;
    }

    camRot.x += 0.001f * renderer->getCursorChangeY();
    camRot.y += 0.001f * renderer->getCursorChangeX();

    renderer->setTransform(camPos, camRot);
    renderer->drawFrame();
  }

  // ORDER IMPORTANT
  World::UnloadWorld();
  World::SetRenderer(nullptr);
  Material::getDefault()->drop();
  Shader::getDefault()->drop();
  renderer->drop();
  // END OF ORDER IMPORTANT
  Chunk::freeEmptyBuffer();
  _CrtDumpMemoryLeaks();
  return 0;
}