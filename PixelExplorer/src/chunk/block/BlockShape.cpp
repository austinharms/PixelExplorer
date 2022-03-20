#include "BlockShape.h"

BlockShape* BlockShape::s_defaultShape = nullptr;
std::unordered_map<std::string, BlockShape*> BlockShape::s_blockShapes;
bool BlockShape::s_shapesLoaded = false;

BlockShape* BlockShape::CreateDefaultShape() {
  BlockShape* block = new BlockShape("DEFAULT");
  block->FullBlock = true;
  block->BlockTransparent = false;
  block->HasPhysicsShape = true;
  block->HasRenderShape = true;
  block->_sharedShapeData = true;
  memset(block->TransparentFace, 0, sizeof(bool) * FACE_COUNT);
  memset(block->RenderIndexCount, 6, sizeof(block->RenderIndexCount));
  memset(block->RenderVertexCount, 4, sizeof(block->RenderVertexCount));
  memset(block->PhysicsIndexCount, 6, sizeof(block->PhysicsIndexCount));
  memset(block->PhysicsVertexCount, 4, sizeof(block->PhysicsVertexCount));
  block->RenderVertices[0] = (float*)malloc(3 * sizeof(float) * 4 * 6);
  block->RenderUVS[0] = (float*)malloc(2 * sizeof(float) * 4 * 6);
  block->RenderIndices[0] = (uint8_t*)malloc(1 * sizeof(uint8_t) * 6 * 6);

  block->RenderVertices[1] = &(block->RenderVertices[0][12]);
  block->RenderVertices[2] = &(block->RenderVertices[1][12]);
  block->RenderVertices[3] = &(block->RenderVertices[2][12]);
  block->RenderVertices[4] = &(block->RenderVertices[3][12]);
  block->RenderVertices[5] = &(block->RenderVertices[4][12]);

  block->RenderIndices[1] = &(block->RenderIndices[0][12]);
  block->RenderIndices[2] = &(block->RenderIndices[1][12]);
  block->RenderIndices[3] = &(block->RenderIndices[2][12]);
  block->RenderIndices[4] = &(block->RenderIndices[3][12]);
  block->RenderIndices[5] = &(block->RenderIndices[4][12]);

  block->RenderUVS[1] = &(block->RenderUVS[0][12]);
  block->RenderUVS[2] = &(block->RenderUVS[1][12]);
  block->RenderUVS[3] = &(block->RenderUVS[2][12]);
  block->RenderUVS[4] = &(block->RenderUVS[3][12]);
  block->RenderUVS[5] = &(block->RenderUVS[4][12]);

  memcpy(block->PhysicsVertices, block->RenderVertices,
         sizeof(block->RenderVertices));
  memcpy(block->PhysicsIndices, block->RenderIndices,
         sizeof(block->RenderIndices));

  float verts[72] = {
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

  float uvs[48] = {
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

  uint8_t indices[36] = {
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

  memcpy(block->RenderVertices[0], verts, sizeof(verts));
  memcpy(block->RenderUVS[0], uvs, sizeof(uvs));
  memcpy(block->RenderIndices[0], indices, sizeof(indices));
  return block;
}

BlockShape::~BlockShape() {
  if (RenderIndices[0] != nullptr) free(RenderIndices[0]);
  if (RenderVertices[0] != nullptr) free(RenderVertices[0]);
  if (RenderUVS[0] != nullptr) free(RenderUVS[0]);
  if (!_sharedShapeData) {
    if (PhysicsIndices[0] != nullptr) free(PhysicsIndices[0]);
    if (PhysicsVertices[0] != nullptr) free(PhysicsVertices[0]);
  }
}

BlockShape* BlockShape::GetShape(std::string shapeName) {
  if (!s_shapesLoaded) return s_defaultShape;
  auto shape = s_blockShapes.find(shapeName);
  if (shape == s_blockShapes.end()) return s_defaultShape;
  return shape->second;
}

bool BlockShape::GetShapesLoaded() { return s_shapesLoaded; }

void BlockShape::LoadShapes() {
  if (s_shapesLoaded) return;
  s_defaultShape = CreateDefaultShape();
  s_shapesLoaded = true;
}

void BlockShape::UnloadShapes() {
  if (!s_shapesLoaded) return;
  s_shapesLoaded = false;
  s_defaultShape->drop();
  s_defaultShape = nullptr;
  for (auto shape : s_blockShapes) shape.second->drop();
  s_blockShapes.clear();
}

BlockShape* BlockShape::GetDefaultShape() { return s_defaultShape; }
