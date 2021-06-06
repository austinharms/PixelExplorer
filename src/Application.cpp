#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

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

  renderer->setCameraTransform(
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -40.0f)));

  // set default material and shader
  {
    Shader* defaultShader = new Shader("./res/shaders/Basic.shader");
    Texture* blockTextures = new Texture("./res/textures/textures.png");
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

  Mesh* mesh = new Mesh();
  //mesh->setMaterial(chunkMaterial);
  mesh->setTransform(
      glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 0.0f)));
  mesh->setIndexCount(36);
  mesh->setVertexCount(24);

  float tw = (1.0f / 96.0f)*16.0f;
  float th = (1.0f / 64.0f)*16.0f;

  float block[] = {
    //Front Face
    -0.5f, -0.5f,  0.5f, tw * 2, th * 1,
    -0.5f,  0.5f,  0.5f, tw * 2, th * 0,
     0.5f,  0.5f,  0.5f, tw * 3, th * 0,
     0.5f, -0.5f,  0.5f, tw * 3, th * 1,

    //Back Face
    -0.5f, -0.5f, -0.5f, tw * 4, th * 1,
    -0.5f,  0.5f, -0.5f, tw * 4, th * 0,
     0.5f,  0.5f, -0.5f, tw * 3, th * 0,
     0.5f, -0.5f, -0.5f, tw * 3, th * 1,

    //Left Face
    -0.5f, -0.5f, -0.5f, tw * 4, th * 1,
    -0.5f,  0.5f, -0.5f, tw * 4, th * 0,
    -0.5f,  0.5f,  0.5f, tw * 5, th * 0,
    -0.5f, -0.5f,  0.5f, tw * 5, th * 1,

    //Right Face
     0.5f, -0.5f, -0.5f, tw * 6, th * 1,
     0.5f,  0.5f, -0.5f, tw * 6, th * 0,
     0.5f,  0.5f,  0.5f, tw * 5, th * 0,
     0.5f, -0.5f,  0.5f, tw * 5, th * 1,

    //Top Face
    -0.5f,  0.5f, -0.5f, tw * 0, th * 0,
    -0.5f,  0.5f,  0.5f, tw * 0, th * 1,
     0.5f,  0.5f,  0.5f, tw * 1, th * 1,
     0.5f,  0.5f, -0.5f, tw * 1, th * 0,

    //Bottom Face
    -0.5f, -0.5f, -0.5f, tw * 1, th * 1,
    -0.5f, -0.5f,  0.5f, tw * 1, th * 0,
     0.5f, -0.5f,  0.5f, tw * 2, th * 0,
     0.5f, -0.5f, -0.5f, tw * 2, th * 1,
  };

  for (short i = 0; i < 120; i += 5) {
    mesh->setVertexPosition(i / 5, block[i], block[i + 1], block[i + 2]);
    mesh->setVertexUV(i / 5, block[i + 3], block[i + 4]);
  }

  unsigned short index[36] = {
       0,  3,  2,  2,  1,  0,  // Front Face
       6,  7,  4,  4,  5,  6,  // Back Face
       8, 11, 10, 10,  9,  8,  // Left Face
      14, 15, 12, 12, 13, 14,  // Right Face
      18, 19, 16, 16, 17, 18,  // Top Face
      20, 23, 22, 22, 21, 20,  // Bottom Face
  };

  for (short i = 0; i < 36; ++i) {
    mesh->setIndex(i, index[i]);
  }

  mesh->updateBuffers();
  renderer->addMesh(mesh);

  Block::BlockFace* faces = new Block::BlockFace[6];
  faces[Block::FRONT].indexCount = 6;
  faces[Block::FRONT].vertexCount = 4;
  faces[Block::FRONT].vertices = new float[12]{
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 
    1.0f, 0.0f, 1.0f
  };
  faces[Block::FRONT].indices = new short[6]{0, 3, 2, 2, 1, 0};
  faces[Block::FRONT].uvs = new float[8]{
    tw * 2, th * 1,
    tw * 2, th * 0,
    tw * 3, th * 0,
    tw * 3, th * 1,
  };

  faces[Block::BACK].indexCount = 6;
  faces[Block::BACK].vertexCount = 4;
  faces[Block::BACK].vertices = new float[12]{
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 
    1.0f, 0.0f, 0.0f
  };
  faces[Block::BACK].indices = new short[6]{2, 3, 0, 0, 1, 2};
  faces[Block::BACK].uvs = new float[8]{
    tw * 4, th * 1,
    tw * 4, th * 0,
    tw * 3, th * 0,
    tw * 3, th * 1,
  };

  faces[Block::LEFT].indexCount = 6;
  faces[Block::LEFT].vertexCount = 4;
  faces[Block::LEFT].vertices = new float[12]{
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 
    0.0f, 0.0f, 1.0f
  };
  faces[Block::LEFT].indices = new short[6]{0, 3, 2, 2, 1, 0};
  faces[Block::LEFT].uvs = new float[8]{
    tw * 4, th * 1,
    tw * 4, th * 0,
    tw * 5, th * 0,
    tw * 5, th * 1,
  };

  
  faces[Block::RIGHT].indexCount = 6;
  faces[Block::RIGHT].vertexCount = 4;
  faces[Block::RIGHT].vertices = new float[12]{
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 
    1.0f, 0.0f, 1.0f
  };
  faces[Block::RIGHT].indices = new short[6]{2, 3, 0, 0, 1, 2};
  faces[Block::RIGHT].uvs = new float[8]{
    tw * 6, th * 1,
    tw * 6, th * 0,
    tw * 5, th * 0,
    tw * 5, th * 1,
  };



  Block* blockPtr = new Block(0, false, faces);
  Block::setDefaultBlock(blockPtr);
  blockPtr->drop();
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
  mesh->drop();
  Block::dropBlocks();
  _CrtDumpMemoryLeaks();
  return 0;
}