#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <stdint.h>
#include <atomic>

class RefCounted {
 public:
  inline RefCounted() : _refCount(1) {}

  inline virtual ~RefCounted() {}

  inline void grab() { ++_refCount; }

  inline bool drop() {
    if (--_refCount == 0) {
      delete this;
      return true;
    } else {
      return false;
    }
  }

  inline uint32_t getRefCount() const { return _refCount; }

 private:
  std::atomic<uint32_t> _refCount;
};

#endif  // !REFCOUNTED_H
