#ifndef DIRECTION_H
#define DIRECTION_H

#include "glm/vec3.hpp"

struct Direction {
 public:
  static const Direction NONE;
  static const Direction FRONT;
  static const Direction BACK;
  static const Direction LEFT;
  static const Direction RIGHT;
  static const Direction TOP;
  static const Direction BOTTOM;
  static const Direction ALL;

  operator uint8_t() const;
  bool operator==(const Direction& d) const;
  bool operator!=(const Direction& d) const;
  Direction operator-(const Direction& d) const;
  Direction operator=(const uint8_t val) const;
  explicit operator glm::vec3() const;
  glm::vec3 ToVec3() const;
  Direction GetOpposite() const;
  ~Direction();

 private:
  Direction(uint8_t value);
  const uint8_t _value;
};
#endif  // !DIRECTION_H
