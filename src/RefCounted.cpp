#include "RefCounted.h"

RefCounted::RefCounted(void (*delFunc)(void *))
    : _refCount(0), _delFunc(delFunc) {}

RefCounted::RefCounted() : _refCount(0), _delFunc(nullptr) {}

RefCounted::~RefCounted() {}

RefCounted *RefCounted::Get() { 
  ++_refCount;
  return this; 
}

bool RefCounted::Drop() { 
  --_refCount;
  if (_refCount > 0) return false;
  if (_delFunc != nullptr) {
    _delFunc(this);
  } else {
    delete this;
  }

  return true;
}
