#pragma once
#include <glm/vec3.hpp>
#include <mutex>
#include <unordered_map>

#include "RefCounted.h"
#include "Chunk.h"

class ChunkManager : public virtual RefCounted {
 public:
  ChunkManager(char*);
  virtual ~ChunkManager();
  void loadChunksInRadiusAsync(glm::vec<3, int> pos, unsigned short radius);
  void saveAllChunks();
  void unloadAllChunks();
  void loadChunk();
 private:
  void unloadChunk();
  Chunk* getChunk(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(glm::vec<3, int> pos);
  void loadChunkAsync(glm::vec<3, int> pos);
  std::unordered_map<std::string, Chunk*> _chunkMap;
};