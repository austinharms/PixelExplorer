//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "World.h"
#include "chunk/Chunk.h"
#include "chunk/block/BaseBlock.h"
#include "rendering/Renderer.h"
#include "rendering/Shader.h"
#include "rendering/TestRenderable.h"
#include "physics/PhysicsManager.h"


int main(void) {
  PhysicsManager::Init();
  Renderer* renderer = new Renderer(1200, 800, "Pixel Explorer V2.0", 60);
  renderer->setCursorHidden(true);
  World::SetRenderer(renderer);
  World::LoadWorld();
  for (uint8_t x = 0; x < 5; ++x)
    for (uint8_t y = 0; y < 5; ++y)
      for (uint8_t z = 0; z < 5; ++z)
        World::GetChunkManager()->LoadChunk(glm::vec<3, int32_t>(x, y, z));
  World::GetChunkManager()->UpdateLoadedChunks();
  //World::GetChunkManager()->LoadChunk(glm::vec<3, int32_t>(0,0,0));
  glm::vec3 camPos(12, 12, 75);
  glm::vec3 camRot(0, 0, 0);
  float moveSpeed = 50;

  while (renderer->getWindowOpen()) {
    if (renderer->getKeyPressed(GLFW_KEY_W)) {
      camPos += renderer->getDeltaTime() * renderer->getForward() * moveSpeed;
    } else if (renderer->getKeyPressed(GLFW_KEY_S)) {
      camPos -= renderer->getDeltaTime() * renderer->getForward() * moveSpeed;
    }

    if (renderer->getKeyPressed(GLFW_KEY_A)) {
      camPos -= renderer->getDeltaTime() *
                glm::cross(renderer->getForward(), glm::vec3(0, 1, 0)) *
                moveSpeed;
    } else if (renderer->getKeyPressed(GLFW_KEY_D)) {
      camPos += renderer->getDeltaTime() *
                glm::cross(renderer->getForward(), glm::vec3(0, 1, 0)) *
                moveSpeed;
    }

    if (renderer->getKeyPressed(GLFW_KEY_SPACE)) {
      camPos.y += renderer->getDeltaTime() * moveSpeed;
    } else if (renderer->getKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
      camPos.y -= renderer->getDeltaTime() * moveSpeed;
    }

    camRot.x += 0.001f * renderer->getCursorChangeY();
    camRot.y += 0.001f * renderer->getCursorChangeX();

    if (renderer->getKeyPressed(GLFW_KEY_ESCAPE))
      renderer->setCursorHidden(false);

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
  PhysicsManager::Destroy();
  Chunk::FreeStaticMembers();
  glfwTerminate();
  //_CrtDumpMemoryLeaks();
  return 0;
}