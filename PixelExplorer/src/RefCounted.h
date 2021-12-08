#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <atomic>
#include <assert.h>

class RefCounted {
 public:
  inline RefCounted(bool shouldDelete = true) : _refCount(1) {
    if (shouldDelete) _refCount += s_deleteMask;
  }

  inline virtual ~RefCounted() {}

  void grab() { 
    //Check for count overflow into delet flag
    assert((_refCount & s_countMask) < s_countMask);
    ++_refCount;
  }

  bool drop() {
    assert((_refCount & s_countMask) > 0);
    --_refCount;
    if ((_refCount & s_countMask) == 0) {
      if ((_refCount & s_deleteMask) != 0)
        delete this;
      return true;
    } else {
      return false;
    }
  }

  unsigned short getRefCount() const { return _refCount & s_countMask; }

 private:
  static const uint32_t s_countMask = 0b01111111111111111111111111111111;
  static const uint32_t s_deleteMask = 0b10000000000000000000000000000000;
  std::atomic<uint32_t> _refCount;
};

#endif  // !REFCOUNTED_H
