#ifndef FACEDIRECTION_H
#define FACEDIRECTION_H

#include <cstdint>

enum class FaceDirection : uint8_t {
  FRONT,
  BACK,
  LEFT,
  RIGHT,
  TOP,
  BOTTOM,
  FACECOUNT,
  NONE,
};

#endif  // !FACEDIRECTION_H
