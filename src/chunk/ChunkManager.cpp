#include "ChunkManager.h"

#include <iostream>

ChunkManager::ChunkManager(const char* chunkPath, Renderer* renderer,
                           int threadPoolSize)
    : _runningThreadCount(0),
      _killRunningThreads(false),
      _renderer(renderer),
      _unloadThread(&ChunkManager::unloadThreadFunction, this),
      _threadPool(),
      _jobQueue() {
  _renderer->grab();
  _unloadThread.detach();
  _threadPool.reserve(threadPoolSize);
  for (int i = 0; i < threadPoolSize; ++i) {
    _threadPool.emplace_back(
        std::thread(&ChunkManager::jobThreadPoolFunction, this));
    _threadPool[i].detach();
  }
}

ChunkManager::~ChunkManager() {
  _killRunningThreads = true;
  _jobCondition.notify_all();
  _renderer->drop();
  unloadAllChunks();
  while (getRunningThreadCount() > 0)
    ;
}

void ChunkManager::loadChunksInRadiusAsync(glm::vec<3, int> pos,
                                           unsigned short radius) {
  Job* job = new Job();
  job->type = Job::LOADR;
  job->pos = pos;
  job->ptr = (void*)radius;
  addJob(job);
}

void ChunkManager::loadChunksInRadius(glm::vec<3, int> pos,
                                      unsigned short radius,
                                      bool useAsyncChunkLoading) {
  unsigned int chunkCount = radius * radius * radius;
  glm::vec<3, int> chunkPos;
  for (unsigned int i = 0; i < chunkCount; ++i) {
    chunkPos.x = (int)(i % radius);
    chunkPos.y = (int)(i / (radius * radius));
    chunkPos.z = (int)((int)(i / radius) % radius);
    if (useAsyncChunkLoading) {
      loadChunkAsync(chunkPos + pos);
    } else {
      loadChunk(chunkPos + pos);
    }
  }
}

void ChunkManager::loadChunkAsync(glm::vec<3, int> pos) {
  Job* job = new Job();
  job->type = Job::LOAD;
  job->pos = pos;
  job->ptr = nullptr;
  addJob(job);
}

void ChunkManager::loadChunk(glm::vec<3, int> pos) {
  Chunk* chunk = getChunkPointer(pos);
  if (chunk != nullptr) {
    chunk->grab();
    Chunk::Status chunkStatus = chunk->getStatus();
    if (chunkStatus == Chunk::LOADED) {
      chunk->setUnloadTime(((unsigned long long int)clock() / CLOCKS_PER_SEC) +
                           10);
    }

    chunk->drop();
    return;
  }

  chunk = new Chunk(pos);
  chunk->generateChunk();
  chunk->updateMesh();
  chunk->setUnloadTime(((unsigned long long int)clock() / CLOCKS_PER_SEC) + 10);
  {
    std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
    _chunkMap.insert({posToString(pos), chunk});
  }
  _renderer->addMesh(chunk->getMesh());
  chunk->setStatus(Chunk::LOADED);
}

void ChunkManager::saveAllChunks() {}

void ChunkManager::unloadAllChunks() {
  for (const std::pair<std::string, Chunk*>& chunkPair : _chunkMap)
    chunkPair.second->drop();
  _chunkMap.clear();
}

void ChunkManager::unloadChunk(glm::vec<3, int> pos) {
  std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
  auto chunk = _chunkMap.find(posToString(pos));
  if (chunk != _chunkMap.end()) {
    chunk->second->setStatus(Chunk::UNLOADING);
    _chunkMap.erase(chunk);
    if (!chunk->second->drop()) {
      chunk->second->setStatus(Chunk::UNLOADED);
    }
  }
}

Chunk* ChunkManager::getChunkPointer(glm::vec<3, int> pos) {
  std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
  auto chunk = _chunkMap.find(posToString(pos));
  if (chunk != _chunkMap.end()) return chunk->second;
  return nullptr;
}

void ChunkManager::jobThreadPoolFunction() {
  incrementRunningThreadCount();
  while (!_killRunningThreads) {
    Job* job = nullptr;

    {
      std::unique_lock<std::mutex> lock(_jobQueueLock);
      _jobCondition.wait(
          lock, [this] { return !_jobQueue.empty() || _killRunningThreads; });
      if (_killRunningThreads) continue;
      job = getNextJob();
      if (job == nullptr) continue;
    }

    switch (job->type) {
      case Job::LOADR:
        loadChunksInRadius(job->pos, (unsigned short)job->ptr, true);
        break;

      case Job::LOAD:
        loadChunk(job->pos);
        break;
    }

    delete job;
  }

  decrementRunningThreadCount();
}

void ChunkManager::unloadThreadFunction() {
  incrementRunningThreadCount();
  while (!_killRunningThreads) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
    std::unordered_map<std::string, Chunk*>::iterator iter = _chunkMap.begin();
    std::unordered_map<std::string, Chunk*>::iterator end = _chunkMap.end();
    unsigned long long int curTime = clock() / CLOCKS_PER_SEC;
    while (iter != end) {
      const std::pair<std::string, Chunk*>& chunkPair = *iter;
      if (chunkPair.second->getUnloadTime() < curTime) {
        chunkPair.second->setStatus(Chunk::UNLOADING);
        iter = _chunkMap.erase(iter);
        if (!chunkPair.second->drop()) {
          chunkPair.second->setStatus(Chunk::UNLOADED);
        }
      } else {
        ++iter;
      }
    }
  }

  decrementRunningThreadCount();
}

Chunk* ChunkManager::getChunk(glm::vec<3, int> pos) { return nullptr; }
