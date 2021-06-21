#include "ChunkManager.h"

ChunkManager::ChunkManager(const char* chunkPath, Renderer* renderer,
                           int jobPoolSize, int loadPoolSize,
                           int maxChunksPerFrame)
    : _runningThreadCount(0),
      _jobPoolSize(jobPoolSize),
      _jobQueueLength(0),
      _loadPoolSize(loadPoolSize),
      _killRunningThreads(false),
      _renderer(renderer),
      _threadPool(),
      _jobQueue(),
      _maxChunkCreationsPerFrame(maxChunksPerFrame),
      _createdChunkQueue(),
      _createdChunkQueueLength(0),
      _chunkCreationRequestCount(0),
      _lastUnloadUpdate(0) {
  _renderer->grab();

  if (jobPoolSize < 1) jobPoolSize = 1;
  if (loadPoolSize < 1) loadPoolSize = 1;
  _threadPool.reserve(jobPoolSize + loadPoolSize);

  for (int i = 0; i < jobPoolSize; ++i) {
    _threadPool.emplace_back(
        std::thread(&ChunkManager::jobThreadPoolFunction, this));
    _threadPool[i].detach();
  }

  for (int i = 0; i < loadPoolSize; ++i) {
    _threadPool.emplace_back(
        std::thread(&ChunkManager::loadThreadPoolFunction, this));
    _threadPool[i + jobPoolSize].detach();
  }
}

ChunkManager::~ChunkManager() {
  _killRunningThreads = true;
  _jobCondition.notify_all();
  _loadAndUnloadCondition.notify_all();
  while (getRunningThreadCount() > 0)
    ;
  unloadAllChunks();
  _renderer->drop();

  while (!_jobQueue.empty()) {
    delete _jobQueue.front();
    _jobQueue.pop();
  }

  while (!_createdChunkQueue.empty()) {
    _createdChunkQueue.front()->drop();
    _createdChunkQueue.pop();
  }
}

void ChunkManager::update() {
  if ((clock() / CLOCKS_PER_SEC) - _lastUnloadUpdate > 1) {
    Job* job = new Job();
    job->type = Job::UPDATEUNLOADING;
    job->pos = glm::vec<3, int>(0);
    addJob(job);
    _lastUnloadUpdate = clock() / CLOCKS_PER_SEC;
  }

  if (_jobQueueLength > 25) {
    std::cout << "Job Queue Overloaded" << std::endl;
  }

  updateChunkCreation();
}

// Chunk Loading
void ChunkManager::loadChunksInRadius(glm::vec<3, int> pos,
                                      unsigned short radius) {
  Job* job = new Job();
  job->type = Job::LOADRADIUS;
  job->pos = pos;
  job->ptr = (void*)radius;
  addJob(job);
}

void ChunkManager::loadThreadPoolFunction() {
  incrementRunningThreadCount();
  while (!_killRunningThreads) {
    glm::vec<3, int> pos(0);

    {
      std::unique_lock<std::mutex> lock(_loadAndUnloadLock);
      _jobCondition.wait(lock, [this] {
        return !_unloadQueue.isQueueEmpty() || !_loadQueue.isQueueEmpty() ||
               _killRunningThreads;
      });
    }

    if (_killRunningThreads) continue;
    if (!_unloadQueue.isQueueEmpty()) {
      if (!_unloadQueue.getNextPosition(&pos)) continue;
      _chunkMapLock.lock_shared();
      Chunk* chunk = getChunkPointer(pos);
      _chunkMapLock.unlock_shared();
      chunk->setStatus(Chunk::UNLOADING);
      _chunkMapLock.lock();
      _chunkMap.erase(posToString(pos));
      _chunkMapLock.unlock();
      chunk->dropAdjacentChunks();
      if (!chunk->drop()) chunk->setStatus(Chunk::UNLOADED);
      _unloadQueue.removePosition(pos);
    } else if (!_loadQueue.isQueueEmpty()) {
      if (!_loadQueue.getNextPosition(&pos)) continue;
      requestChunkCreation();
      Chunk* chunk = nullptr;
      do {
        chunk = getCreatedChunk();
        if (chunk == nullptr)
          std::this_thread::sleep_for(std::chrono::microseconds(250));
      } while ((!_killRunningThreads) && chunk == nullptr);

      if (_killRunningThreads) {
        if (chunk != nullptr) chunk->drop();
        continue;
      }

      chunk->setChunkPosition(pos);

      {
        std::lock_guard<std::shared_mutex> locker(_chunkMapLock);
        _chunkMap.insert({posToString(pos), chunk});
      }

      chunk->generateChunk();
      chunk->setAdjacentChunk(Chunk::FRONT,
                              getChunk(pos + glm::vec<3, int>(0, 0, 1)));
      chunk->setAdjacentChunk(Chunk::BACK,
                              getChunk(pos + glm::vec<3, int>(0, 0, -1)));
      chunk->setAdjacentChunk(Chunk::LEFT,
                              getChunk(pos + glm::vec<3, int>(-1, 0, 0)));
      chunk->setAdjacentChunk(Chunk::RIGHT,
                              getChunk(pos + glm::vec<3, int>(1, 0, 0)));
      chunk->setAdjacentChunk(Chunk::TOP,
                              getChunk(pos + glm::vec<3, int>(0, 1, 0)));
      chunk->setAdjacentChunk(Chunk::BOTTOM,
                              getChunk(pos + glm::vec<3, int>(0, -1, 0)));
      chunk->updateMesh();
      chunk->setUnloadTime(((unsigned long long int)clock() / CLOCKS_PER_SEC) +
                           10);

      _renderer->addMesh(chunk->getMesh());
      chunk->setStatus(Chunk::LOADED);
      _loadQueue.removePosition(pos);
    }
  }

  decrementRunningThreadCount();
}

// Job Pool
void ChunkManager::jobThreadPoolFunction() {
  incrementRunningThreadCount();
  while (!_killRunningThreads) {
    Job* job = nullptr;

    {
      std::unique_lock<std::mutex> lock(_jobQueueLock);
      _jobCondition.wait(
          lock, [this] { return _jobQueueLength > 0 || _killRunningThreads; });
      if (_killRunningThreads) continue;
      job = getNextJob();
      if (job == nullptr) continue;
    }

    switch (job->type) {
      case Job::LOADRADIUS: {
        unsigned short radius = (unsigned short)job->ptr;
        unsigned int chunkCount = radius * radius * radius;
        glm::vec<3, int> chunkPos;
        for (unsigned int i = 0; i < chunkCount; ++i) {
          chunkPos.x = (int)(i % radius);
          chunkPos.y = (int)(i / (radius * radius));
          chunkPos.z = (int)((int)(i / radius) % radius);
          _chunkMapLock.lock_shared();
          Chunk* chunk = getChunkPointer(chunkPos + job->pos);
          _chunkMapLock.unlock_shared();
          Chunk::Status status = getChunkStatus(chunk);
          if (status == Chunk::UNLOADED) {
            _loadQueue.addPosition(chunkPos + job->pos);
          } else if (status == Chunk::LOADED) {
            chunk->setUnloadTime(
                (unsigned long long int)(clock() / CLOCKS_PER_SEC) + 10);
          }
        }
      } break;

      case Job::UPDATEUNLOADING: {
        _chunkMapLock.lock_shared();
        std::unordered_map<std::string, Chunk*>::iterator iter =
            _chunkMap.begin();
        std::unordered_map<std::string, Chunk*>::iterator end = _chunkMap.end();
        unsigned long long int curTime = clock() / CLOCKS_PER_SEC;
        while (iter != end) {
          const std::pair<std::string, Chunk*>& chunkPair = *iter;
          if (getChunkStatus(chunkPair.second) == Chunk::LOADED &&
              chunkPair.second->getUnloadTime() < curTime)
            _unloadQueue.addPosition(chunkPair.second->getPosition());
          ++iter;
        }
        _chunkMapLock.unlock_shared();
      } break;
    }

    delete job;
  }

  decrementRunningThreadCount();
}

// Main Thread Chunk Creation
void ChunkManager::updateChunkCreation() {
  if (_chunkCreationRequestCount > 0 && _createChunkLock.try_lock()) {
    int createdChunkCount = 0;
    while (_chunkCreationRequestCount > 0 &&
           (_maxChunkCreationsPerFrame == -1 ||
            createdChunkCount < _maxChunkCreationsPerFrame)) {
      _createdChunkQueue.emplace(new Chunk());
      --_chunkCreationRequestCount;
      ++_createdChunkQueueLength;
      ++createdChunkCount;
    }

    _createChunkLock.unlock();
  }
}

// Chunk Helpers
Chunk* ChunkManager::getChunkPointer(glm::vec<3, int> pos) {
  auto chunk = _chunkMap.find(posToString(pos));
  if (chunk != _chunkMap.end()) return chunk->second;
  return nullptr;
}

Chunk::Status ChunkManager::getChunkStatus(glm::vec<3, int> pos) {
  Chunk* chunk = getChunkPointer(pos);
  if (chunk == nullptr) return Chunk::UNLOADED;
  // if (chunk == _chunkPlaceholderPointer) return Chunk::LOADING;
  return chunk->getStatus();
}

Chunk::Status ChunkManager::getChunkStatus(Chunk* chunk) {
  if (chunk == nullptr) return Chunk::UNLOADED;
  // if (chunk == _chunkPlaceholderPointer) return Chunk::LOADING;
  return chunk->getStatus();
}

Chunk* ChunkManager::getLoadedChunk(glm::vec<3, int> pos) {
  Chunk* chunk = getChunkPointer(pos);
  if (chunk != nullptr && chunk->getStatus() == Chunk::LOADED) return chunk;
  return nullptr;
}

Chunk* ChunkManager::getChunk(glm::vec<3, int> pos) {
  Chunk* chunk = getChunkPointer(pos);
  if (getChunkStatus(chunk) >= Chunk::LOADING) return chunk;
  return nullptr;
}

void ChunkManager::saveAllChunks() {}

void ChunkManager::unloadAllChunks() {
  _loadQueue.clear();
  _unloadQueue.clear();
  std::lock_guard<std::shared_mutex> lock(_chunkMapLock);
  for (const std::pair<std::string, Chunk*>& chunkPair : _chunkMap) {
    // if (chunkPair.second != _chunkPlaceholderPointer)
    // chunkPair.second->drop();
    chunkPair.second->dropAdjacentChunks();
    chunkPair.second->drop();
  }
  _chunkMap.clear();
}