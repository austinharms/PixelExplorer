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
#include "rendering/TestRenderable.h"
#include "World.h"
#include "chunk/block/BaseBlock.h"
#include "chunk/Chunk.h"
#define GLM_ENABLE_EXPERIMENTAL

int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Pixel Explorer V2.0");
  renderer->setCursorHidden(false);
  World::LoadWorld();
  BaseBlock::LoadBlockManifest();
  TestRenderable* testObj = new TestRenderable(); 
  renderer->addRenderable(testObj);
  testObj->drop();
  Chunk* testChunk = new Chunk();
  testChunk->updateMesh();
  renderer->addRenderable(testChunk);
  testChunk->drop();
  testChunk = nullptr;
  while (renderer->getWindowOpen()) {
    renderer->drawFrame();
  }

  //ORDER IMPORTANT
  Material::getDefault()->drop();
  Shader::getDefault()->drop();
  renderer->drop();
  BaseBlock::UnloadBlocks();
  //END OF ORDER IMPORTANT
  Chunk::freeEmptyBuffer();
  _CrtDumpMemoryLeaks();
  return 0;
}