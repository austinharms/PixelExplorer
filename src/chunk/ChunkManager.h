#pragma once
#include <condition_variable>
#include <glm/vec3.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Chunk.h"
#include "RefCounted.h"
#include "Renderer.h"

class ChunkManager : public virtual RefCounted {
 public:
  ChunkManager(const char* chunkPath, Renderer* renderer, int jobPoolSize = 5,
               int loadPoolSize = 20, int maxChunksPerFrame = 50);
  virtual ~ChunkManager();
  void loadChunksInRadius(glm::vec<3, int> pos, unsigned short radius);
  void saveAllChunks();
  void unloadAllChunks();
  void update();  // MUST be ran on main thread with openGL context

 private:
  Renderer* _renderer;
  unsigned long long int _lastUnloadUpdate;

  // Threading
  std::mutex _threadCountLock;
  std::vector<std::thread> _threadPool;
  int _runningThreadCount;
  bool _killRunningThreads;

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

  // Job Pool
  struct Job {
    enum JobType { LOADRADIUS, UPDATEUNLOADING };
    JobType type;
    glm::vec<3, int> pos;
    void* ptr;
  };

  std::mutex _jobQueueLock;
  std::queue<Job*> _jobQueue;
  int _jobQueueLength;
  std::condition_variable _jobCondition;
  int _jobPoolSize;
  void jobThreadPoolFunction();

  void addJob(Job* job) {
    {
      std::lock_guard<std::mutex> lock(_jobQueueLock);
      _jobQueue.emplace(job);
      ++_jobQueueLength;
    }
    _jobCondition.notify_one();
  }

  Job* getNextJob() {
    if (_jobQueueLength < 1) return nullptr;
    Job* job = _jobQueue.front();
    --_jobQueueLength;
    _jobQueue.pop();
    return job;
  }

  // Chunk Loading
  std::recursive_mutex _chunkMapLock;
  std::unordered_map<std::string, Chunk*> _chunkMap;
  void* _chunkPlaceholderPointer;
  std::mutex _loadChunkLock;
  std::queue<glm::vec<3, int>> _loadChunkQueue;
  int _loadChunkQueueLength;
  std::condition_variable _loadCondition;
  int _loadPoolSize;
  void loadThreadPoolFunction();

  void loadChunk(glm::vec<3, int> pos) {
    {
      std::lock_guard<std::mutex> lock(_loadChunkLock);
      _loadChunkQueue.emplace(pos);
      ++_loadChunkQueueLength;
    }
    _loadCondition.notify_one();
  }

  bool getChunkLoadPos(glm::vec<3, int>* pos) {
    if (_loadChunkQueueLength == 0) return false;
    *pos = _loadChunkQueue.front();
    _loadChunkQueue.pop();
    --_loadChunkQueueLength;
    return true;
  }

  // Main Thread Chunk Creation
  std::mutex _createChunkLock;
  std::queue<Chunk*> _createdChunkQueue;
  int _maxChunkCreationsPerFrame;
  int _chunkCreationRequestCount;
  int _createdChunkQueueLength;
  void updateChunkCreation();

  void requestChunkCreation() {
    _createChunkLock.lock();
    ++_chunkCreationRequestCount;
    _createChunkLock.unlock();
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
  Chunk* getChunk(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(Chunk* chunk);

  std::string posToString(glm::vec<3, int> pos) {
    return std::to_string(pos.x) + "-" + std::to_string(pos.y) + "-" +
           std::to_string(pos.z);
  }
};