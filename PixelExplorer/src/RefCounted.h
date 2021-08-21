#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <atomic>
#include <assert.h>

class RefCounted {
 public:
  inline RefCounted() : _refCount(1) {}
  inline virtual ~RefCounted() {}

  void grab() { ++_refCount; }

  bool drop() {
    assert(_refCount > 0);
    --_refCount;
    if (_refCount == 0) {
      delete this;
      return true;
    } else {
      return false;
    }
  }

  unsigned short getRefCount() const { return _refCount; }

 private:
  std::atomic<unsigned long int> _refCount;
};

#endif  // !REFCOUNTED_H
