#define _CRTDBG_MAP_ALLOC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <crtdbg.h>
#include <stdlib.h>

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "Material.h"
#include "Player.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "chunk/Block.h"
#include "chunk/Chunk.h"
#include "chunk/ChunkManager.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Test");
  if (!renderer->renderInit()) return -1;

  renderer->hideCursor(true);
  // renderer->setCameraTransform(glm::translate(glm::mat4(1.0f),
  // glm::vec3(-20.0f, -20.0f, -80.0f)));
  Player* player = new Player(renderer);
  // set default material and shader
  {
    Shader* defaultShader = new Shader("./res/shaders/Basic.shader");
    Texture* blockTextures = new Texture("./res/textures/default.png");
    Material* defaultMaterial = new Material(defaultShader, blockTextures);
    defaultShader->drop();
    renderer->setDefaultMaterial(defaultMaterial);
    defaultMaterial->drop();
    blockTextures->drop();
  }

  // create block material
  {
    Shader* chunkShader = new Shader("./res/shaders/Chunk.shader");
    Texture* blockTextures = new Texture("./res/textures/textures.png");
    Material* blockMaterial = new Material(chunkShader, blockTextures);
    chunkShader->drop();
    blockTextures->drop();
    Chunk::setBlockMaterial(blockMaterial);
    blockMaterial->drop();
  }

  Block::loadBlocks("./res/blocks.dat");
  Block::setDefaultBlock(Block::getBlock(0));

  ChunkManager* chunkManager = new ChunkManager(nullptr, renderer, 4, 5, -1);

  //chunkManager->loadChunksInRadius(glm::vec<3, int>(0, 0, 0), 100);
  chunkManager->loadChunksInRadius(glm::vec<3, int>(0, 0, 0), 1);
  unsigned long long int nextUpdateTime = 0;
  while (renderer->windowOpen()) {
    if ((unsigned long long int)(clock() / CLOCKS_PER_SEC) >= nextUpdateTime) {
      chunkManager->loadChunksInRadius(glm::vec<3, int>(0, 5, 0), 25);
      nextUpdateTime = (unsigned long long int)(clock() / CLOCKS_PER_SEC) + 5;
    }
    chunkManager->update();
    player->update();
    renderer->render();
  }

  chunkManager->drop();
  player->drop();
  renderer->drop();
  Chunk::setBlockMaterial(nullptr);
  Block::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}