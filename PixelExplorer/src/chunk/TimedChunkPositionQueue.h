#pragma once

#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>
#include <mutex>
#include <queue>
#include <unordered_set>

class TimedChunkPositionQueue {
 public:
  TimedChunkPositionQueue();
  ~TimedChunkPositionQueue();
  bool addPosition(glm::vec<3, int> pos, int timeoutDelay = 4);
  bool getNextPosition(glm::vec<3, int>* pos);
  void removePosition(glm::vec<3, int> pos);
  int getQueueLength() const { return _length; }
  bool isQueueEmpty() const { return _length < 1; }
  void clear();

 private:
  struct Position {
    glm::vec<3, int> pos;
    int timeout;
  };
  std::mutex _positionLock;
  std::unordered_set<glm::vec<3, int>> _positionSet;
  std::queue<Position> _positionQueue;
  int _length;
};