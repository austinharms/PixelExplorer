#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Material.h"
#include "Player.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "chunk/Chunk.h"
#include "chunk/ChunkManager.h"
#include "chunk/block/Blocks.h"
#include "chunk/generator/ChunkGenerator.h"
#include "chunk/generator/FlatChunkGenerator.h"
#include "chunk/generator/PerlinChunkGenerator.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "physics/PhysicsCommonRef.h"
#include "physics/PhysicsWorldRef.h"
#include "reactphysics3d/reactphysics3d.h"
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

  PhysicsCommonRef* physicsCommon = new PhysicsCommonRef();
  PhysicsWorldRef* phyWorld = nullptr;
  {
    rp3d::PhysicsWorld::WorldSettings settings;
    settings.defaultVelocitySolverNbIterations = 20;
    settings.isSleepingEnabled = false;
    settings.gravity = rp3d::Vector3(0, -9.81, 0);
    phyWorld = physicsCommon->createPhysicsWorldRef(settings);
  }

  ChunkGenerator* chunkGen = new PerlinChunkGenerator(458679840956);
  // ChunkGenerator* chunkGen = new FlatChunkGenerator(2, 0);
  // int threadCount = std::thread::hardware_concurrency();
  // if (threadCount < 3) threadCount = 3;
  //--threadCount;
  ChunkManager* chunkManager =
      new ChunkManager("world\\D0\\", renderer, chunkGen, phyWorld, 12, 6, -1);
  chunkGen->drop();

  unsigned long long int nextUpdateTime = 0;
  glm::vec<3, int> lastChunkPos(0);

  float phyAccumulator = 0.0f;
  const float phyStep = 1.0f / 60.0f;

  while (renderer->windowOpen()) {
    glm::vec<3, int> playerChunkPos =
        ChunkManager::vec3ToChunkSpace(player->getPosition());
    if ((unsigned long long int)(clock() / CLOCKS_PER_SEC) >= nextUpdateTime ||
        playerChunkPos != lastChunkPos) {
      // chunkManager->loadChunksInRadius(playerChunkPos, 0);
      chunkManager->loadChunksInRadius(glm::vec3(0), 15);
      lastChunkPos = playerChunkPos;
      nextUpdateTime = (unsigned long long int)(clock() / CLOCKS_PER_SEC) + 2;
      std::cout << "Chunk Load Update: X:" << playerChunkPos.x
                << " Y:" << playerChunkPos.y << " Z:" << playerChunkPos.z
                << std::endl;
    }

    chunkManager->update();

    phyAccumulator += renderer->getDeltaTime();
    while (phyAccumulator > phyStep) {
      phyWorld->getPhysicsWorld()->update(phyStep);
      phyAccumulator -= phyStep;
    }

    player->update();
    renderer->render();
  }

  phyWorld->drop();
  chunkManager->drop();
  physicsCommon->drop();
  player->drop();
  renderer->drop();
  Chunk::setBlockMaterial(nullptr);
  Blocks::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}