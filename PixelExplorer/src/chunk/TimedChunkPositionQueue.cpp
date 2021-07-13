#include "TimedChunkPositionQueue.h"

TimedChunkPositionQueue::TimedChunkPositionQueue() : _length(0) {}

TimedChunkPositionQueue::~TimedChunkPositionQueue() {}

bool TimedChunkPositionQueue::addPosition(glm::vec<3, int> pos,
                                          int timeoutDelay) {
  std::lock_guard<std::mutex> lock(_positionLock);
  if (!_positionSet.insert(pos).second) return false;
  _positionQueue.emplace(Position{pos, (clock() / CLOCKS_PER_SEC) + timeoutDelay});
  ++_length;
  return true;
}

bool TimedChunkPositionQueue::getNextPosition(glm::vec<3, int>* pos) {
  while (true) {
    _positionLock.lock();
    if (isQueueEmpty()) {
      _positionLock.unlock();
      return false;
    }
    if (_positionQueue.front().timeout >= clock() / CLOCKS_PER_SEC) {
      *pos = _positionQueue.front().pos;
      _positionQueue.pop();
      --_length;
      _positionLock.unlock();
      return true;
    } else {
      _positionSet.erase(_positionQueue.front().pos);
      _positionQueue.pop();
      --_length;
      _positionLock.unlock();
    }
  }
}

void TimedChunkPositionQueue::removePosition(glm::vec<3, int> pos) {
  std::lock_guard<std::mutex> lock(_positionLock);
  _positionSet.erase(pos);
}

void TimedChunkPositionQueue::clear() {
  std::lock_guard<std::mutex> lock(_positionLock);
  _positionSet.clear();
  while (!_positionQueue.empty()) _positionQueue.pop();
}
