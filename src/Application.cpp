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
#include "chunk/block/Blocks.h"
#include "chunk/ChunkManager.h"
#include "chunk/Chunk.h"
#include "glm/glm.hpp"
#include "chunk/generator/PerlinChunkGenerator.h"
#include "chunk/generator/FlatChunkGenerator.h"
#include "chunk/generator/ChunkGenerator.h"
#define GLM_ENABLE_EXPERIMENTAL

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Pixel Explorer");
  if (!renderer->renderInit()) return -1;

  renderer->hideCursor(true);
  Player* player = new Player(renderer);
  // set default material and shader
  {
    Shader* defaultShader = new Shader("res\\shaders\\Basic.shader");
    Texture* blockTextures = new Texture("res\\textures\\default.png");
    Material* defaultMaterial = new Material(defaultShader, blockTextures);
    defaultShader->drop();
    renderer->setDefaultMaterial(defaultMaterial);
    defaultMaterial->drop();
    blockTextures->drop();
  }

  // create block material
  {
    Shader* chunkShader = new Shader("res\\shaders\\Chunk.shader");
    Texture* blockTextures = new Texture("res\\textures\\textures.png");
    Material* blockMaterial = new Material(chunkShader, blockTextures);
    chunkShader->drop();
    blockTextures->drop();
    Chunk::setBlockMaterial(blockMaterial);
    blockMaterial->drop();
  }

  Blocks::loadBlocks("res\\blocks.dat");
  Blocks::setDefaultBlock(Blocks::getBlock(1));

  ChunkGenerator* chunkGen = new PerlinChunkGenerator(458679840956);
  //ChunkGenerator* chunkGen = new FlatChunkGenerator(2, 0);
  ChunkManager* chunkManager =
      new ChunkManager("world\\D0\\", renderer, chunkGen, 12, 6, -1);
  chunkGen->drop();

  unsigned long long int nextUpdateTime = 0;
  glm::vec<3, int> lastChunkPos(0);
  while (renderer->windowOpen()) {
    glm::vec<3, int> playerChunkPos =
        ChunkManager::vec3ToChunkSpace(player->getPosition());
    if ((unsigned long long int)(clock() / CLOCKS_PER_SEC) >= nextUpdateTime ||
        playerChunkPos != lastChunkPos) {
      chunkManager->loadChunksInRadius(playerChunkPos, 5);
      lastChunkPos = playerChunkPos;
      nextUpdateTime = (unsigned long long int)(clock() / CLOCKS_PER_SEC) + 2;
       std::cout << "Chunk Load Update: X:" << playerChunkPos.x
                << " Y:" << playerChunkPos.y << " Z:" << playerChunkPos.z
                << std::endl;
    }

    chunkManager->update();
    player->update();
    renderer->render();
  }

  chunkManager->drop();
  player->drop();
  renderer->drop();
  Chunk::setBlockMaterial(nullptr);
  Blocks::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}