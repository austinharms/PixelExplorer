#include "ChunkPositionQueue.h"

ChunkPositionQueue::ChunkPositionQueue() : _length(0) {}

ChunkPositionQueue::~ChunkPositionQueue() {}

bool ChunkPositionQueue::addPosition(glm::vec<3, int> pos) {
  std::lock_guard<std::mutex> lock(_positionLock);
  if (!_positionSet.insert(pos).second) return false;
  _positionQueue.emplace(pos);
  ++_length;
  return true;
}

bool ChunkPositionQueue::getNextPosition(glm::vec<3, int>* pos) {
  std::lock_guard<std::mutex> lock(_positionLock);
  if (isQueueEmpty()) return false;
  *pos = _positionQueue.front();
  _positionQueue.pop();
  --_length;
  return true;
}

void ChunkPositionQueue::removePosition(glm::vec<3, int> pos) {
  std::lock_guard<std::mutex> lock(_positionLock);
  _positionSet.erase(pos);
}

void ChunkPositionQueue::clear() {
  std::lock_guard<std::mutex> lock(_positionLock);
  _positionSet.clear();
  while (!_positionQueue.empty()) _positionQueue.pop();
}
