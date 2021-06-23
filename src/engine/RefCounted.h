#pragma once
class RefCounted {
 public:
  RefCounted(void (*delFunc)(void*)) : _refCount(1) {}
  RefCounted() : _refCount(1) {}
  virtual ~RefCounted(){};
  void grab() const { ++_refCount; }
  bool drop() {
    --_refCount;
    if (_refCount > 0) return false;
    delete this;
    return true;
  }
  short getRefCount() const { return _refCount; }

 private:
  mutable int _refCount;
};