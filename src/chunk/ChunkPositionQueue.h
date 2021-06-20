#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>
#include <mutex>
#include <queue>
#include <unordered_set>

class ChunkPositionQueue {
 public:
  ChunkPositionQueue();
  ~ChunkPositionQueue();
  bool addPosition(glm::vec<3, int> pos);
  bool getNextPosition(glm::vec<3, int>* pos);
  void removePosition(glm::vec<3, int> pos);
  int getQueueLength() const { return _length; }
  bool isQueueEmpty() const { return _length < 1; }
  void clear();

 private:
  std::mutex _positionLock;
  std::unordered_set<glm::vec<3, int>> _positionSet;
  std::queue<glm::vec<3, int>> _positionQueue;
  int _length;
};