#include "ChunkManager.h"

ChunkManager::ChunkManager(const char* chunkPath, Renderer* renderer,
                           int jobPoolSize, int loadPoolSize,
                           int maxChunksPerFrame)
    : _runningThreadCount(0),
      _jobPoolSize(jobPoolSize),
      _jobQueueLength(0),
      _loadPoolSize(loadPoolSize),
      _loadChunkQueueLength(0),
      _killRunningThreads(false),
      _renderer(renderer),
      _threadPool(),
      _jobQueue(),
      _maxChunkCreationsPerFrame(maxChunksPerFrame),
      _createdChunkQueue(),
      _createdChunkQueueLength(0),
      _chunkCreationRequestCount(0),
      _lastUnloadUpdate(0) {
  _chunkPlaceholderPointer = malloc(1);
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
  _loadCondition.notify_all();
  while (getRunningThreadCount() > 0)
    ;
  unloadAllChunks();
  _renderer->drop();
  free(_chunkPlaceholderPointer);

  while (!_jobQueue.empty()) {
    delete _jobQueue.front();
    _jobQueue.pop();
  }

  while (!_loadChunkQueue.empty()) {
    _loadChunkQueue.pop();
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

  if (_jobQueueLength > 100) {
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
      std::unique_lock<std::mutex> lock(_loadChunkLock);
      _jobCondition.wait(lock, [this] {
        return _loadChunkQueueLength > 0 || _killRunningThreads;
      });
      if (_killRunningThreads) continue;
      if (!getChunkLoadPos(&pos)) continue;
    }

    _chunkMapLock.lock();
    Chunk* chunk = getChunkPointer(pos);

    // if chunk is already loaded
    if (chunk != nullptr) {
      _chunkMapLock.unlock();
      continue;
    }

    _chunkMap.insert({posToString(pos), (Chunk*)_chunkPlaceholderPointer});
    _chunkMapLock.unlock();
    requestChunkCreation();
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
    chunk->generateChunk();
    chunk->updateMesh();
    chunk->setUnloadTime(((unsigned long long int)clock() / CLOCKS_PER_SEC) +
                         10);
    {
      std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
      _chunkMap.insert_or_assign(posToString(pos), chunk);
    }

    _renderer->addMesh(chunk->getMesh());
    chunk->setStatus(Chunk::LOADED);
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
          Chunk* chunk = getChunkPointer(chunkPos + job->pos);
          Chunk::Status status = getChunkStatus(chunk);
          if (status == Chunk::UNLOADED) {
            loadChunk(chunkPos);
          } else if (status == Chunk::LOADED) {
            chunk->setUnloadTime((clock() / CLOCKS_PER_SEC) + 10);
          }
        }
      } break;

      case Job::UPDATEUNLOADING: {
        std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
        std::unordered_map<std::string, Chunk*>::iterator iter =
            _chunkMap.begin();
        std::unordered_map<std::string, Chunk*>::iterator end = _chunkMap.end();
        unsigned long long int curTime = clock() / CLOCKS_PER_SEC;
        while (iter != end) {
          const std::pair<std::string, Chunk*>& chunkPair = *iter;
          if (getChunkStatus(chunkPair.second) == Chunk::LOADED &&
              chunkPair.second->getUnloadTime() < curTime) {
            chunkPair.second->setStatus(Chunk::UNLOADING);
            iter = _chunkMap.erase(iter);
            if (!chunkPair.second->drop())
              chunkPair.second->setStatus(Chunk::UNLOADED);
          } else {
            ++iter;
          }
        }
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
  std::lock_guard<std::recursive_mutex> locker(_chunkMapLock);
  auto chunk = _chunkMap.find(posToString(pos));
  if (chunk != _chunkMap.end()) return chunk->second;
  return nullptr;
}

Chunk::Status ChunkManager::getChunkStatus(glm::vec<3, int> pos) {
  Chunk* chunk = getChunkPointer(pos);
  if (chunk == nullptr) return Chunk::UNLOADED;
  if (chunk == _chunkPlaceholderPointer) return Chunk::LOADING;
  return chunk->getStatus();
}

Chunk::Status ChunkManager::getChunkStatus(Chunk* chunk) {
  if (chunk == nullptr) return Chunk::UNLOADED;
  if (chunk == _chunkPlaceholderPointer) return Chunk::LOADING;
  return chunk->getStatus();
}

Chunk* ChunkManager::getChunk(glm::vec<3, int> pos) {
  Chunk* chunk = getChunkPointer(pos);
  if (chunk != nullptr && chunk->getStatus() == Chunk::LOADED) return chunk;
  return nullptr;
}

void ChunkManager::saveAllChunks() {}

void ChunkManager::unloadAllChunks() {
  for (const std::pair<std::string, Chunk*>& chunkPair : _chunkMap) {
    if (chunkPair.second != _chunkPlaceholderPointer) chunkPair.second->drop();
  }
  _chunkMap.clear();
}