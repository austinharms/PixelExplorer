#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "RefCounted.h"

class Renderable : virtual RefCounted {
 public:
  inline Renderable() {}

  inline virtual ~Renderable() {}

  virtual bool getVisible();
  virtual void onRender() = 0;
  virtual bool onPreRender(
      float deltaTime, float* cameraPos,
      float* cameraRotation) = 0;  // Should update and return mesh visibility
  virtual void onPostRender() = 0;

 private:
};
#endif  // !RENDERABLE_H
