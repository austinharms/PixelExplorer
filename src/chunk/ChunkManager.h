#pragma once
#include <condition_variable>
#include <glm/vec3.hpp>
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
  ChunkManager(const char* chunkPath, Renderer* renderer,
               int threadPoolSize = 1);
  virtual ~ChunkManager();
  void loadChunksInRadiusAsync(glm::vec<3, int> pos, unsigned short radius);
  void loadChunksInRadius(glm::vec<3, int> pos, unsigned short radius, bool useAsyncChunkLoading = false);
  void saveAllChunks();
  void unloadAllChunks();

 private:
  struct Job {
    enum JobType { LOAD, LOADR };
    JobType type;
    glm::vec<3, int> pos;
    void* ptr;
  };

  Renderer* _renderer;
  std::recursive_mutex _chunkMapLock;
  std::unordered_map<std::string, Chunk*> _chunkMap;
  std::mutex _threadCountLock;
  int _runningThreadCount;
  bool _killRunningThreads;
  std::thread _unloadThread;
  std::mutex _jobQueueLock;
  std::vector<std::thread> _threadPool;
  std::queue<Job*> _jobQueue;
  std::condition_variable _jobCondition;

  void unloadThreadFunction();
  void jobThreadPoolFunction();
  void unloadChunk(glm::vec<3, int> pos);
  Chunk* getChunk(glm::vec<3, int> pos);
  Chunk* getChunkPointer(glm::vec<3, int> pos);
  Chunk::Status getChunkStatus(glm::vec<3, int> pos);
  void loadChunkAsync(glm::vec<3, int> pos);
  void loadChunk(glm::vec<3, int> pos);

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

  void addJob(Job* job) {
    _jobQueueLock.lock();
    _jobQueue.emplace(job);
    _jobQueueLock.unlock();
    _jobCondition.notify_one();
  }

  Job* getNextJob() { 
    if (_jobQueue.empty()) return nullptr;
    Job* job = _jobQueue.front();
    _jobQueue.pop();
    return job;
  }

  std::string posToString(glm::vec<3, int> pos) {
    return std::to_string(pos.x) + "-" + std::to_string(pos.y) + "-" +
           std::to_string(pos.z);
  }
};