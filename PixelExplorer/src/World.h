#ifndef WORLD_H
#define WORLD_H

#include "RefCounted.h"

class World : public RefCounted {
 public:
  static World* LoadWorld();

  virtual ~World();

 private:
  World();
};
#endif