#pragma once
class RefCounted {
 public:
  RefCounted(void (*delFunc)(void*));
  RefCounted();
  ~RefCounted();
  RefCounted* Get();
  bool Drop();
  short GetRefCount() const { return _refCount; }
 private:
  void (*_delFunc)(void*);
  short _refCount;
};