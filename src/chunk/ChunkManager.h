#pragma once
#include <glm/vec3.hpp>
#include <mutex>
#include <string>
#include <unordered_map>
#include <thread>

#include "Chunk.h"
#include "RefCounted.h"
#include "Renderer.h"

class ChunkManager : public virtual RefCounted {
 public:
  ChunkManager(const char* chunkPath, Renderer* renderer);
  virtual ~ChunkManager();
  void loadChunksInRadiusAsync(glm::vec<3, int> pos, unsigned short radius);
  void loadChunksInRadius(glm::vec<3, int> pos, unsigned short radius);
  void saveAllChunks();
  void unloadAllChunks();
  void loadChunk(glm::vec<3, int> pos);

 private:
  Renderer* _renderer;
  std::recursive_mutex _chunkMapLock;
  std::unordered_map<std::string, Chunk*> _chunkMap;
  std::mutex _threadCountLock;
  int _runningThreadCount;
  bool _killRunningThreads;
  std::thread unloadThread;

  void unloadThreadFunction();
  void unloadChunk(glm::vec<3, int> pos);
  Chunk* getChunk(glm::vec<3, int> pos);
  Chunk* getChunkPointer(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(glm::vec<3, int> pos);
  void loadChunkAsync(glm::vec<3, int> pos);

  int getRunningThreadCount() {
    std::lock_guard<std::mutex> locker(_threadCountLock);
    return _runningThreadCount;
  }

  void incrementRunningThreadCount() {
    std::lock_guard<std::mutex> locker(_threadCountLock);
    ++_runningThreadCount;
  }

  void decrementRunningThreadCount() {
    std::lock_guard<std::mutex> locker(_threadCountLock);
    --_runningThreadCount;
  }

  std::string posToString(glm::vec<3, int> pos) {
    return std::to_string(pos.x) + "-" + std::to_string(pos.y) + "-" +
           std::to_string(pos.z);
  }
};