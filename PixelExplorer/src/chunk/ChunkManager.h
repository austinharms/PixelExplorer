#pragma once
#include <atomic>
#include <condition_variable>
#include <glm/vec3.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Chunk.h"
#include "ChunkPositionQueue.h"
#include "RefCounted.h"
#include "Renderer.h"
#include "TimedChunkPositionQueue.h"
#include "generator/ChunkGenerator.h"
#include "../physics/PhysicsWorldRef.h"
#include "reactphysics3d/reactphysics3d.h"

class ChunkManager : public virtual RefCounted {
 public:
  ChunkManager(const char* chunkPath, Renderer* renderer,
               ChunkGenerator* generator, PhysicsWorldRef* physicsWorld, int jobPoolSize = 5,
               int loadPoolSize = 6, int maxChunksPerFrame = 2);
  virtual ~ChunkManager();
  void loadChunksInRadius(glm::vec<3, int> pos, unsigned short radius);
  void saveAllChunks();
  void unloadAllChunks();
  void update();  // MUST be ran on main thread with openGL context
  static glm::vec<3, int> vec3ToChunkSpace(glm::vec3 pos) {
    return glm::vec<3, int>(pos.x / Chunk::CHUNK_SIZE,
                            pos.y / Chunk::CHUNK_SIZE,
                            pos.z / Chunk::CHUNK_SIZE);
  }

 private:
  Renderer* _renderer;
  ChunkGenerator* _generator;
  const char* _savePath;
  PhysicsWorldRef* _physicsWorld;
  unsigned long long int _lastUnloadUpdate;

  // Threading
  std::vector<std::thread> _threadPool;
  std::atomic<int> _runningThreadCount;
  std::atomic<bool> _killRunningThreads;

  // Job Pool
  struct Job {
    enum JobType { LOADRADIUS, UPDATEUNLOADING, UPDATECHUNK };
    JobType type;
    glm::vec<3, int> pos;
    void* ptr;
    ~Job() {
      if (this->type == UPDATECHUNK) ((Chunk*)ptr)->drop();
    }
  };

  std::mutex _jobQueueLock;
  std::queue<Job*> _jobQueue;
  int _jobQueueLength;
  std::condition_variable _jobCondition;
  int _jobPoolSize;
  void jobThreadPoolFunction();
  void addUpdateChunkJob(Chunk* chunk);

  void addJob(Job* job) {
    {
      std::lock_guard<std::mutex> lock(_jobQueueLock);
      _jobQueue.emplace(job);
      ++_jobQueueLength;
    }
    _jobCondition.notify_all();
  }

  Job* getNextJob() {
    if (_jobQueueLength < 1) return nullptr;
    Job* job = _jobQueue.front();
    --_jobQueueLength;
    _jobQueue.pop();
    return job;
  }

  // Chunk Loading
  std::shared_mutex _chunkMapLock;
  std::unordered_map<std::string, Chunk*> _chunkMap;
  ChunkPositionQueue _unloadQueue;
  TimedChunkPositionQueue _loadQueue;
  std::mutex _loadAndUnloadLock;
  std::condition_variable _loadAndUnloadCondition;
  int _loadPoolSize;
  void loadThreadPoolFunction();

  // Main Thread Chunk Creation
  std::mutex _createChunkLock;
  std::queue<Chunk*> _createdChunkQueue;
  int _maxChunkCreationsPerFrame;
  std::atomic<int> _chunkCreationRequestCount;
  std::atomic<int> _createdChunkQueueLength;
  void updateChunkCreation();
  void checkAndLoadChunk(glm::vec<3, int> pos);

  void requestChunkCreation() { ++_chunkCreationRequestCount; }

  Chunk* getCreatedChunk() {
    std::lock_guard<std::mutex> lock(_createChunkLock);
    if (_createdChunkQueueLength < 1) return nullptr;
    Chunk* chunk = _createdChunkQueue.front();
    _createdChunkQueue.pop();
    --_createdChunkQueueLength;
    return chunk;
  }

  // Chunk Helpers
  Chunk* getChunkPointer(glm::vec<3, int> pos);
  Chunk* getLoadedChunk(glm::vec<3, int> pos);
  Chunk* getChunk(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(Chunk* chunk);
};