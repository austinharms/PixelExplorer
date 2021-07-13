#pragma once
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "RefCounted.h"

class BlockBase : public virtual RefCounted {
 public:
  enum Face { TOP = 0, BOTTOM = 1, FRONT = 2, BACK = 3, LEFT = 4, RIGHT = 5 };
  struct BlockFace {
    BlockFace() {
      vertexCount = 0;
      indexCount = 0;
      vertices = nullptr;
      uvs = nullptr;
      indices = nullptr;
      fullFace = true;
    }

    ~BlockFace() {
      delete[] vertices;
      delete[] uvs;
      delete[] indices;
    }

    float* vertices;
    float* uvs;
    unsigned char* indices;
    unsigned char vertexCount;
    unsigned char indexCount;
    bool fullFace;
  };

  BlockBase(uint32_t id, bool transparent, BlockFace* blockFaces)
      : _id(id), _transparent(transparent), _faces(blockFaces) {}

  virtual ~BlockBase() { delete[] _faces; }

  uint32_t getID() const { return _id; }

  BlockFace* getBlockFace(Face f) const { return &_faces[(int)f]; }

  bool getFullBlockFace(Face f) const { return &_faces[(int)f].fullFace; }

  bool getTransparent() const { return _transparent; }

 private:
  BlockFace* _faces;
  int32_t _id;
  bool _transparent;
};