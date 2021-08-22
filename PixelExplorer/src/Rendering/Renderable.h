#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "RefCounted.h"

class Renderable : public virtual RefCounted {
 public:
  inline Renderable() : _visible(true), _dropFlag(false), _id(++s_idCounter) {}

  inline virtual ~Renderable() {}

  bool getVisible() const { return _visible; }

  unsigned short getId() const { return _id; }

  bool getRendererDropFlag() const { return _dropFlag; }

  void setRendererDropFlag(bool flag) { _dropFlag = flag; }

  virtual bool onPreRender(
      float deltaTime, float* cameraPos,
      float* cameraRotation) = 0;  // Should update and return mesh visibility
  virtual void getMaterial() = 0;
  virtual void getTransform() = 0;
  virtual void onRender() = 0;

 private:
  static unsigned short s_idCounter;
  bool _visible;
  bool _dropFlag;
  unsigned short _id;
};
#endif  // !RENDERABLE_H
