#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Material.h"
#include "PhysicsMesh.h"
#include "Player.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "chunk/block/Blocks.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "reactphysics3d/reactphysics3d.h"
#define GLM_ENABLE_EXPERIMENTAL

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Pixel Explorer - Physics");
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

  Blocks::loadBlocks("res\\blocks.dat");

  rp3d::PhysicsCommon physicsCommon;
  rp3d::PhysicsWorld* world = physicsCommon.createPhysicsWorld();

  rp3d::Vector3 position(0, 0, 0);
  rp3d::Quaternion orientation = rp3d::Quaternion::identity();
  rp3d::Transform transform(position, orientation);
  PhysicsMesh* phyMesh = new PhysicsMesh(world, transform);
  BlockBase* b = Blocks::getBlock(2);
  int inCount = 0;
  int vtCount = 0;
  for (int f = 0; f < 6; ++f) {
    inCount += b->getBlockFace((BlockBase::Face)f)->indexCount;
    vtCount += b->getBlockFace((BlockBase::Face)f)->vertexCount;
  }

  phyMesh->setIndexCount(inCount);
  phyMesh->setVertexCount(vtCount);
  inCount = 0;
  vtCount = 0;
  for (int f = 0; f < 6; ++f) {
    for (int v = 0; v < b->getBlockFace((BlockBase::Face)f)->vertexCount; ++v) {
      phyMesh->setVertexPosition(
          vtCount++, b->getBlockFace((BlockBase::Face)f)->vertices[0 + (v * 3)],
          b->getBlockFace((BlockBase::Face)f)->vertices[1 + (v * 3)],
          b->getBlockFace((BlockBase::Face)f)->vertices[2 + (v * 3)]);
    }

    for (int i = 0; i < b->getBlockFace((BlockBase::Face)f)->indexCount; ++i) {
      phyMesh->setIndex(inCount++,
                        b->getBlockFace((BlockBase::Face)f)->indices[i]);
    }
  }
  phyMesh->updateBuffers();
  renderer->addMesh(phyMesh);
  phyMesh->drop();

  const float phyStep = 1.0 / 60.0;
  float phyAccumulator = 0;
  world->update(phyStep);
  while (renderer->windowOpen()) {
    player->update();
    renderer->render();

    phyAccumulator += renderer->getDeltaTime();
    while (phyAccumulator >= phyStep) {
      //world->update(phyStep);
      phyAccumulator -= phyStep;
    }
  }

  player->drop();
  renderer->drop();
  Blocks::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}