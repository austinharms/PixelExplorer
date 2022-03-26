#include "BlockShape.h"
#include "Logger.h"

BlockShape* BlockShape::s_defaultShape = nullptr;
std::unordered_map<std::string, BlockShape*> BlockShape::s_blockShapes;
bool BlockShape::s_shapesLoaded = false;

BlockShape::BlockShape(std::string name) : Name(name) {
  RenderIndices[0] = nullptr;
  RenderVertices[0] = nullptr;
  PhysicsIndices[0] = nullptr;
  PhysicsVertices[0] = nullptr;
}

BlockShape::~BlockShape() {
  if (RenderIndices[0] != nullptr) free(RenderIndices[0]);
  if (RenderVertices[0] != nullptr) free(RenderVertices[0]);
  if (PhysicsIndices[0] != nullptr) free(PhysicsIndices[0]);
  if (PhysicsVertices[0] != nullptr) free(PhysicsVertices[0]);
}

BlockShape* BlockShape::createDefaultShape() {
    const float verts[72] = {
    // Front
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // Back
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    // Left
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    // Right
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,

    // Top
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Bottom
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
  };

  const float uvs[48] = {
    // Front
    0,0,
    1,0,
    1,1,
    0,1,

    // Back
    0,0,
    1,0,
    1,1,
    0,1,

    // Left
    0,0,
    1,0,
    1,1,
    0,1,

    // Right
    0,0,
    1,0,
    1,1,
    0,1,

    // Top
    0,0,
    1,0,
    1,1,
    0,1,

    // Bottom
    0,0,
    1,0,
    1,1,
    0,1,
  };

  const uint8_t indices[36] = {
    // Front
    0,1,2,
    0,2,3,

    // Back
    2,1,0,
    2,3,0,

    // Left
    0,1,2,
    0,2,3,

    // Right
    2,1,0,
    2,3,0,

    // Top
    0,1,2,
    0,2,3,

    // Bottom
    2,1,0,
    2,3,0,
  };

  BlockShape* block = new BlockShape("DEFAULT");
  block->FullBlock = true;
  block->BlockTransparent = false;
  block->HasPhysicsShape = true;
  block->HasRenderShape = true;
  memset(block->TransparentFace, 0, sizeof(bool) * FACE_COUNT);
  memset(block->RenderIndexCount, 6, sizeof(block->RenderIndexCount));
  memset(block->RenderVertexCount, 4, sizeof(block->RenderVertexCount));
  memset(block->PhysicsIndexCount, 6, sizeof(block->PhysicsIndexCount));
  memset(block->PhysicsVertexCount, 4, sizeof(block->PhysicsVertexCount));

  block->RenderVertices[0] = (float*)malloc(sizeof(verts) + sizeof(uvs));
  if (block->RenderVertices[0] == nullptr)
    Logger::fatal(
        "Failed to allocate vertex render buffer for DEFAULT block shape");
  block->RenderVertices[1] = block->RenderVertices[0] + 20;
  block->RenderVertices[2] = block->RenderVertices[1] + 20;
  block->RenderVertices[3] = block->RenderVertices[2] + 20;
  block->RenderVertices[4] = block->RenderVertices[3] + 20;
  block->RenderVertices[5] = block->RenderVertices[4] + 20;

  block->RenderIndices[0] = (uint8_t*)malloc(sizeof(indices));
  if (block->RenderIndices[0] == nullptr)
    Logger::fatal(
        "Failed to allocate index render buffer for DEFAULT block shape");
  block->RenderIndices[1] = block->RenderIndices[0] + 6;
  block->RenderIndices[2] = block->RenderIndices[1] + 6;
  block->RenderIndices[3] = block->RenderIndices[2] + 6;
  block->RenderIndices[4] = block->RenderIndices[3] + 6;
  block->RenderIndices[5] = block->RenderIndices[4] + 6;

    block->PhysicsVertices[0] = (float*)malloc(sizeof(verts));
  if (block->PhysicsVertices[0] == nullptr)
    Logger::fatal(
        "Failed to allocate vertex physics buffer for DEFAULT block shape");
  block->PhysicsVertices[1] = block->PhysicsVertices[0] + 12;
  block->PhysicsVertices[2] = block->PhysicsVertices[1] + 12;
  block->PhysicsVertices[3] = block->PhysicsVertices[2] + 12;
  block->PhysicsVertices[4] = block->PhysicsVertices[3] + 12;
  block->PhysicsVertices[5] = block->PhysicsVertices[4] + 12;

  block->PhysicsIndices[0] = (uint8_t*)malloc(sizeof(indices));
  if (block->PhysicsIndices[0] == nullptr)
    Logger::fatal(
        "Failed to allocate index physics buffer for DEFAULT block shape");
  block->PhysicsIndices[1] = block->PhysicsIndices[0] + 6;
  block->PhysicsIndices[2] = block->PhysicsIndices[1] + 6;
  block->PhysicsIndices[3] = block->PhysicsIndices[2] + 6;
  block->PhysicsIndices[4] = block->PhysicsIndices[3] + 6;
  block->PhysicsIndices[5] = block->PhysicsIndices[4] + 6;

  memcpy(block->PhysicsVertices[0], verts, sizeof(verts));
  memcpy(block->PhysicsIndices[0], indices, sizeof(indices));
  memcpy(block->RenderIndices[0], indices, sizeof(indices));
  for (int32_t i = 0; i < 24; ++i) {
    block->RenderVertices[0][(i * 5) + 0] = verts[(i * 3) + 0];
    block->RenderVertices[0][(i * 5) + 1] = verts[(i * 3) + 1];
    block->RenderVertices[0][(i * 5) + 2] = verts[(i * 3) + 2];
    block->RenderVertices[0][(i * 5) + 3] = uvs[(i * 2) + 0];
    block->RenderVertices[0][(i * 5) + 4] = uvs[(i * 2) + 1];
  }

  Logger::debug("Created DEFAULT Block Shape");
  return block;
}

BlockShape* BlockShape::getShape(std::string shapeName) {
  if (!s_shapesLoaded) return s_defaultShape;
  auto shape = s_blockShapes.find(shapeName);
  if (shape == s_blockShapes.end()) return s_defaultShape;
  return shape->second;
}

bool BlockShape::getShapesLoaded() { return s_shapesLoaded; }

void BlockShape::loadShapes() {
  if (s_shapesLoaded) return;
  Logger::info("Loading Block Shapes");
  s_defaultShape = createDefaultShape();
  s_shapesLoaded = true;
  Logger::info("Done Loading Block Shapes");
}

void BlockShape::unloadShapes() {
  if (!s_shapesLoaded) return;
  Logger::info("Unloading Block Shapes");
  s_shapesLoaded = false;
  s_defaultShape->drop();
  s_defaultShape = nullptr;
  for (auto shape : s_blockShapes) shape.second->drop();
  s_blockShapes.clear();
  Logger::info("Done Unloading Block Shapes");
}

BlockShape* BlockShape::getDefaultShape() { return s_defaultShape; }
