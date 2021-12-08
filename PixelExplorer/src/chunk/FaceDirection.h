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

#ifndef FACEDIRECTIONFLAG_H
#define FACEDIRECTIONFLAG_H

#include <cstdint>

#include "glm/vec3.hpp"

class FaceDirectionFlag {
 public:
  static const uint8_t NONE = 0b00000000;
  static const uint8_t FRONT = 0b00000001;
  static const uint8_t BACK = 0b00000010;
  static const uint8_t LEFT = 0b00000100;
  static const uint8_t RIGHT = 0b00001000;
  static const uint8_t TOP = 0b00010000;
  static const uint8_t BOTTOM = 0b00100000;
  static const uint8_t ALL = 0b00111111;

  static const uint8_t DirectionToFlag(FaceDirection dir) {
    switch (dir) {
      case FaceDirection::FRONT:
        return FRONT;
      case FaceDirection::BACK:
        return BACK;
      case FaceDirection::LEFT:
        return LEFT;
      case FaceDirection::RIGHT:
        return RIGHT;
      case FaceDirection::TOP:
        return TOP;
      case FaceDirection::BOTTOM:
        return BOTTOM;
      case FaceDirection::FACECOUNT:
      case FaceDirection::NONE:
      default:
        return NONE;
        break;
    }
  }

  static const FaceDirection GetOppositeDirection(FaceDirection dir) {
    switch (dir) {
      case FaceDirection::FRONT:
        return FaceDirection::BACK;
      case FaceDirection::BACK:
        return FaceDirection::FRONT;
      case FaceDirection::LEFT:
        return FaceDirection::RIGHT;
      case FaceDirection::RIGHT:
        return FaceDirection::LEFT;
      case FaceDirection::TOP:
        return FaceDirection::BOTTOM;
      case FaceDirection::BOTTOM:
        return FaceDirection::TOP;
      case FaceDirection::FACECOUNT:
      case FaceDirection::NONE:
      default:
        return FaceDirection::NONE;
        break;
    }
  }

  static const FaceDirection GetOppositeDirection(uint8_t dir) {
    return GetOppositeDirection((FaceDirection)dir);
  }

  static const glm::vec3 DirectionToVector(FaceDirection dir) {
    switch (dir) {
      case FaceDirection::FRONT:
        return glm::vec3(0, 0, 1);
      case FaceDirection::BACK:
        return glm::vec3(0, 0, -1);
      case FaceDirection::LEFT:
        return glm::vec3(-1, 0, 0);
      case FaceDirection::RIGHT:
        return glm::vec3(1, 0, 0);
      case FaceDirection::TOP:
        return glm::vec3(0, 1, 0);
      case FaceDirection::BOTTOM:
        return glm::vec3(0, -1, 0);
      case FaceDirection::FACECOUNT:
      case FaceDirection::NONE:
      default:
        return glm::vec3(0, 0, 0);
        break;
    }
  }

  static const glm::vec<3, int32_t> DirectionToInt32Vector(FaceDirection dir) {
    switch (dir) {
      case FaceDirection::FRONT:
        return glm::vec<3, int32_t>(0, 0, 1);
      case FaceDirection::BACK:
        return glm::vec<3, int32_t>(0, 0, -1);
      case FaceDirection::LEFT:
        return glm::vec<3, int32_t>(-1, 0, 0);
      case FaceDirection::RIGHT:
        return glm::vec<3, int32_t>(1, 0, 0);
      case FaceDirection::TOP:
        return glm::vec<3, int32_t>(0, 1, 0);
      case FaceDirection::BOTTOM:
        return glm::vec<3, int32_t>(0, -1, 0);
      case FaceDirection::FACECOUNT:
      case FaceDirection::NONE:
      default:
        return glm::vec<3, int32_t>(0, 0, 0);
        break;
    }
  }

  static const uint8_t DirectionToFlag(char dir) {
    return DirectionToFlag((FaceDirection)dir);
  }

  static const uint8_t DirectionToFlag(uint8_t dir) {
    return DirectionToFlag((FaceDirection)dir);
  }

 private:
  FaceDirectionFlag() {}
  ~FaceDirectionFlag() {}
};

#endif  // !FACEDIRECTIONFLAG_H
