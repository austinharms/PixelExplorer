#pragma once
#include <atomic>

class RefCounted {
 public:
  RefCounted(void (*delFunc)(void*)) : _refCount(1) {}
  RefCounted() : _refCount(1) {}
  virtual ~RefCounted() {}
  void grab() {
    ++_refCount;
  }

  bool drop() {
    --_refCount;
    if (_refCount > 0) {
      return false;
    }

    delete this;
    return true;
  }

  int getRefCount() {
    return _refCount;
  }

 private:
  std::atomic<int> _refCount;
};