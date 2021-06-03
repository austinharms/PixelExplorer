#pragma once
class RefCounted {
 public:
  RefCounted(void (*delFunc)(void*)) : _refCount(1), _delFunc(delFunc) {}
  RefCounted() : _refCount(1), _delFunc(nullptr) {}
  virtual ~RefCounted(){};
  void grab() const { ++_refCount; }
  bool drop() {
    --_refCount;
    if (_refCount > 0) return false;
    if (_delFunc != nullptr)
      _delFunc(this);
    delete this;
    return true;
  }
  short getRefCount() const { return _refCount; }

 private:
  void (*_delFunc)(void*);
  mutable short _refCount;
};