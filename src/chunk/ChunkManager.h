#pragma once
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
#include <atomic>

#include "Chunk.h"
#include "ChunkGenerator.h"
#include "ChunkPositionQueue.h"
#include "RefCounted.h"
#include "Renderer.h"

class ChunkManager : public virtual RefCounted {
 public:
  ChunkManager(const char* chunkPath, Renderer* renderer,
               ChunkGenerator* generator, int jobPoolSize = 5,
               int loadPoolSize = 6, int maxChunksPerFrame = 2);
  virtual ~ChunkManager();
  void loadChunksInRadius(glm::vec<3, int> pos, unsigned short radius);
  void saveAllChunks();
  void unloadAllChunks();
  void update();  // MUST be ran on main thread with openGL context

 private:
  Renderer* _renderer;
  ChunkGenerator* _generator;
  std::string _savePath;
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
  ChunkPositionQueue _loadQueue;
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

  void requestChunkCreation() {
    ++_chunkCreationRequestCount;
  }

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

  std::string posToString(glm::vec<3, int> pos) {
    return std::to_string(pos.x) + "-" + std::to_string(pos.y) + "-" +
           std::to_string(pos.z);
  }
};