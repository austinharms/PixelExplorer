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
#include "chunk/BlockBase.h"
#include "chunk/Chunk.h"
#include "chunk/ChunkManager.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Test");
  if (!renderer->renderInit()) return -1;

  renderer->hideCursor(true);
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

  BlockBase::loadBlocks("./res/blocks.dat");
  BlockBase::setDefaultBlock(BlockBase::getBlock(0));

  ChunkManager* chunkManager = new ChunkManager("./world/D0/", renderer, 10, 20, -1);

  unsigned long long int nextUpdateTime = 0;
  while (renderer->windowOpen()) {
    if ((unsigned long long int)(clock() / CLOCKS_PER_SEC) >= nextUpdateTime) {
      chunkManager->loadChunksInRadius(glm::vec<3, int>(0, 0, 0), 5);
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
  BlockBase::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}