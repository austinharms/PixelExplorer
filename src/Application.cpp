#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "chunk/Chunk.h"
#include "chunk/Block.h"


int main(void) {
  Renderer* renderer = new Renderer(800, 600, "Test");
  if (!renderer->renderInit()) return -1;

  renderer->setCameraTransform(glm::rotate(
      glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, -20.0f, -80.0f)),
      glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

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

  Material* chunkMaterial = nullptr;
  //create chunk material
  {
    Shader* chunkShader = new Shader("./res/shaders/Chunk.shader");
    Texture* blockTextures = new Texture("./res/textures/textures.png");
    chunkMaterial = new Material(chunkShader, blockTextures);
    chunkShader->drop();
    blockTextures->drop();
  }

  Block::loadBlocks("./res/blocks.dat");
  Block::setDefaultBlock(Block::getBlock(0));

  Chunk* chunk = new Chunk(glm::vec<3, int>(0, 0, 0), chunkMaterial);
  chunkMaterial->drop();
  chunk->generateChunk();
  chunk->updateMesh();
  renderer->addMesh(chunk->getMesh());

  while (renderer->windowOpen()) {
    renderer->render();
  }

  renderer->drop();
  chunk->drop();
  Block::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}