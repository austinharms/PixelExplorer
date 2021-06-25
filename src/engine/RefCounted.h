#pragma once
#include <mutex>

//#define LOCK_REFCOUNTER

class RefCounted {
 public:
  RefCounted(void (*delFunc)(void*)) : _refCount(1) {}
  RefCounted() : _refCount(1) {}
  virtual ~RefCounted() {}
  void grab() {
#ifdef LOCK_REFCOUNTER
    std::lock_guard<std::mutex> lock(_lock);
#endif  // LOCK_REFCOUNTER
    ++_refCount;
  }

  bool drop() {
#ifdef LOCK_REFCOUNTER
    _lock.lock();
#endif  // LOCK_REFCOUNTER
    --_refCount;
    if (_refCount > 0) {
#ifdef LOCK_REFCOUNTER
      _lock.unlock();
#endif  // LOCK_REFCOUNTER
      return false;
    }
#ifdef LOCK_REFCOUNTER
    _lock.unlock();
#endif  // LOCK_REFCOUNTER
    delete this;
    return true;
  }

  int getRefCount() {
#ifdef LOCK_REFCOUNTER
    std::lock_guard<std::mutex> lock(_lock);
#endif  // LOCK_REFCOUNTER
    return _refCount;
  }

 private:
  int _refCount;
#ifdef LOCK_REFCOUNTER
  std::mutex _lock;
#endif  // LOCK_REFCOUNTER
};