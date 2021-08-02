#include "ChunkManager.h"

//#define LOG_QUEUE_LENGTH

ChunkManager::ChunkManager(const char* chunkPath, Renderer* renderer,
                           ChunkGenerator* generator,
                           PhysicsWorldRef* physicsWorld, int jobPoolSize,
                           int loadPoolSize, int maxChunksPerFrame)
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
      _lastUnloadUpdate(0),
      _savePath(chunkPath),
      _generator(generator),
      _physicsWorld(physicsWorld) {
  _renderer->grab();
  _generator->grab();
  _physicsWorld->grab();
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
  while (_runningThreadCount > 0)
    ;
  unloadAllChunks();
  _renderer->drop();
  _generator->drop();
  _physicsWorld->drop();

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
#ifdef LOG_QUEUE_LENGTH
  std::cout << "Job Queue: " << _jobQueueLength
            << " UnloadQueue: " << _unloadQueue.getQueueLength()
            << " Load Queue: " << _loadQueue.getQueueLength()
            << " Loaded Chunk Count: " << _chunkMap.size() << std::endl;
#endif  // LOG_QUEUE_LENGTH

  // if (_jobQueueLength > 200) {
  //  std::cout << "Job Queue Overloaded" << std::endl;
  //}

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
  ++_runningThreadCount;
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
      _chunkMap.erase(Chunk::posToString(pos));
      _chunkMapLock.unlock();
      chunk->dropAdjacentChunks();
      chunk->saveChunk(_savePath);
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
        _chunkMap.insert({Chunk::posToString(pos), chunk});
      }

      chunk->loadChunk(_savePath, _generator);
      chunk->setUnloadTime(((unsigned long long int)clock() / CLOCKS_PER_SEC) +
                           6);
      chunk->setStatus(Chunk::LOADED);

      Chunk* adjChunk = nullptr;
      if (chunk->getAdjacentChunk(Chunk::FRONT) == nullptr) {
        _chunkMapLock.lock_shared();
        adjChunk = getChunk(pos + glm::vec<3, int>(0, 0, 1));
        _chunkMapLock.unlock_shared();
        if (adjChunk != nullptr) {
          chunk->setAdjacentChunk(Chunk::FRONT, adjChunk);
          adjChunk->setAdjacentChunk(Chunk::BACK, chunk);
          addUpdateChunkJob(adjChunk);
        }
      }

      if (chunk->getAdjacentChunk(Chunk::BACK) == nullptr) {
        _chunkMapLock.lock_shared();
        adjChunk = getChunk(pos + glm::vec<3, int>(0, 0, -1));
        _chunkMapLock.unlock_shared();
        if (adjChunk != nullptr) {
          chunk->setAdjacentChunk(Chunk::BACK, adjChunk);
          adjChunk->setAdjacentChunk(Chunk::FRONT, chunk);
          addUpdateChunkJob(adjChunk);
        }
      }

      if (chunk->getAdjacentChunk(Chunk::LEFT) == nullptr) {
        _chunkMapLock.lock_shared();
        adjChunk = getChunk(pos + glm::vec<3, int>(-1, 0, 0));
        _chunkMapLock.unlock_shared();
        if (adjChunk != nullptr) {
          chunk->setAdjacentChunk(Chunk::LEFT, adjChunk);
          adjChunk->setAdjacentChunk(Chunk::RIGHT, chunk);
          addUpdateChunkJob(adjChunk);
        }
      }

      if (chunk->getAdjacentChunk(Chunk::RIGHT) == nullptr) {
        _chunkMapLock.lock_shared();
        adjChunk = getChunk(pos + glm::vec<3, int>(1, 0, 0));
        _chunkMapLock.unlock_shared();
        if (adjChunk != nullptr) {
          chunk->setAdjacentChunk(Chunk::RIGHT, adjChunk);
          adjChunk->setAdjacentChunk(Chunk::LEFT, chunk);
          addUpdateChunkJob(adjChunk);
        }
      }

      if (chunk->getAdjacentChunk(Chunk::TOP) == nullptr) {
        _chunkMapLock.lock_shared();
        adjChunk = getChunk(pos + glm::vec<3, int>(0, 1, 0));
        _chunkMapLock.unlock_shared();
        if (adjChunk != nullptr) {
          chunk->setAdjacentChunk(Chunk::TOP, adjChunk);
          adjChunk->setAdjacentChunk(Chunk::BOTTOM, chunk);
          addUpdateChunkJob(adjChunk);
        }
      }

      if (chunk->getAdjacentChunk(Chunk::BOTTOM) == nullptr) {
        _chunkMapLock.lock_shared();
        adjChunk = getChunk(pos + glm::vec<3, int>(0, -1, 0));
        _chunkMapLock.unlock_shared();
        if (adjChunk != nullptr) {
          chunk->setAdjacentChunk(Chunk::BOTTOM, adjChunk);
          adjChunk->setAdjacentChunk(Chunk::TOP, chunk);
          addUpdateChunkJob(adjChunk);
        }
      }

      chunk->updateMesh();
      _renderer->addMesh(chunk->getMesh());
      _loadQueue.removePosition(pos);
    }
  }

  --_runningThreadCount;
}

// Job Pool
void ChunkManager::jobThreadPoolFunction() {
  ++_runningThreadCount;
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
        int radius = (int)((unsigned short)job->ptr);
        glm::vec<3, int> chunkPos;
        int size = 3;
        int dist = 1;
        int offset;
        checkAndLoadChunk(job->pos);
        for (int i = 0; i < radius; ++i) {
          for (int j = 0; j < size * size; ++j) {
            offset = (size - 1) / 2;
            chunkPos.z = dist;
            chunkPos.x = (j % size) - offset;
            chunkPos.y = (j / size) - offset;
            checkAndLoadChunk(chunkPos + job->pos);
            chunkPos.z = -dist;
            chunkPos.x = (j % size) - offset;
            chunkPos.y = (j / size) - offset;
            checkAndLoadChunk(chunkPos + job->pos);
            chunkPos.z = (j % size) - offset;
            chunkPos.x = -dist;
            chunkPos.y = (j / size) - offset;
            checkAndLoadChunk(chunkPos + job->pos);
            chunkPos.z = (j % size) - offset;
            chunkPos.x = dist;
            chunkPos.y = (j / size) - offset;
            checkAndLoadChunk(chunkPos + job->pos);
            chunkPos.z = (j % size) - offset;
            chunkPos.x = (j / size) - offset;
            chunkPos.y = dist;
            checkAndLoadChunk(chunkPos + job->pos);
            chunkPos.z = (j % size) - offset;
            chunkPos.x = (j / size) - offset;
            chunkPos.y = -dist;
            checkAndLoadChunk(chunkPos + job->pos);
          }
          ++dist;
          size += 2;
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
              chunkPair.second->getUnloadTime() < curTime) {
            _unloadQueue.addPosition(chunkPair.second->getPosition());
            _loadAndUnloadCondition.notify_one();
          }
          ++iter;
        }
        _chunkMapLock.unlock_shared();
      } break;

      case Job::UPDATECHUNK: {
        if (getChunkStatus((Chunk*)job->ptr) >= Chunk::LOADING &&
            ((Chunk*)job->ptr)->getChunkModified()) {
          ((Chunk*)job->ptr)->updateMesh();
        }
      } break;
    }

    delete job;
  }

  --_runningThreadCount;
}

void ChunkManager::addUpdateChunkJob(Chunk* chunk) {
  chunk->grab();
  Job* job = new Job();
  job->ptr = chunk;
  job->type = Job::UPDATECHUNK;
  addJob(job);
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

void ChunkManager::checkAndLoadChunk(glm::vec<3, int> pos) {
  _chunkMapLock.lock_shared();
  Chunk* chunk = getChunkPointer(pos);
  _chunkMapLock.unlock_shared();
  Chunk::Status status = getChunkStatus(chunk);
  if (status == Chunk::UNLOADED) {
    _loadQueue.addPosition(pos);
    _loadAndUnloadCondition.notify_one();
  } else if (status == Chunk::LOADED) {
    chunk->setUnloadTime((unsigned long long int)(clock() / CLOCKS_PER_SEC) +
                         5);
  }
}

// Chunk Helpers
Chunk* ChunkManager::getChunkPointer(glm::vec<3, int> pos) {
  auto chunk = _chunkMap.find(Chunk::posToString(pos));
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
    chunkPair.second->saveChunk(_savePath);
    chunkPair.second->drop();
  }

  _chunkMap.clear();
}