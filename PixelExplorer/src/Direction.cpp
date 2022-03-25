#include "Direction.h"

const Direction Direction::NONE = 0b00000000;
const Direction Direction::FRONT = 0b00000001;
const Direction Direction::BACK = 0b00000010;
const Direction Direction::LEFT = 0b00000100;
const Direction Direction::RIGHT = 0b00001000;
const Direction Direction::TOP = 0b00010000;
const Direction Direction::BOTTOM = 0b00100000;
const Direction Direction::ALL = 0b00111111;

Direction::Direction(uint8_t value) : _value(value) {}

Direction::~Direction() {}

inline Direction::operator uint8_t() const { return _value; }

inline bool Direction::operator==(const Direction& d) const {
  return d._value == _value;
}

inline bool Direction::operator!=(const Direction& d) const {
  return d._value != _value;
}

inline Direction Direction::operator-(const Direction& d) const {
  return GetOpposite();
}

inline Direction Direction::operator=(const uint8_t val) const {
  return Direction(val);
}

inline Direction::operator glm::vec3() const { return ToVec3(); }

glm::vec3 Direction::ToVec3() const {
  switch (_value) {
    case 0b00000001:  // FRONT
      return glm::vec3(0, 0, 1);

    case 0b00000010:  // BACK
      return glm::vec3(0, 0, -1);

    case 0b00000100:  // LEFT
      return glm::vec3(-1, 0, 0);

    case 0b00001000:  // RIGHT
      return glm::vec3(1, 0, 0);

    case 0b00010000:  // TOP
      return glm::vec3(0, 1, 0);

    case 0b00100000:  // BOTTOM
      return glm::vec3(0, -1, 0);

    case 0b00111111:  // ALL
      return glm::vec3(1, 1, 1);

    case 0b00000000:  // NONE
      return glm::vec3(0, 0, 0);
  }
}

Direction Direction::GetOpposite() const {
  switch (_value) {
    case 0b00000001:  // FRONT
      return BACK;

    case 0b00000010:  // BACK
      return FRONT;

    case 0b00000100:  // LEFT
      return RIGHT;

    case 0b00001000:  // RIGHT
      return LEFT;

    case 0b00010000:  // TOP
      return BOTTOM;

    case 0b00100000:  // BOTTOM
      return TOP;

    case 0b00111111:  // ALL
      return NONE;

    case 0b00000000:  // NONE
      return ALL;
  }
}
