#ifndef DIRECTION_H
#define DIRECTION_H

#include <stdint.h>

#include "glm/vec3.hpp"

namespace px::util {
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
  static const uint8_t DIRECTION_COUNT = 6;

  static Direction fromIndex(int32_t index);

  operator uint8_t() const;
  bool operator==(const Direction& d) const;
  bool operator!=(const Direction& d) const;
  Direction operator-(const Direction& d) const;
  Direction operator=(const uint8_t val) const;
  explicit operator glm::vec3() const;
  explicit operator glm::ivec3() const;
  glm::vec3 toVec3() const;
  glm::ivec3 toIVec3() const;
  int32_t toIndex() const;
  Direction getOpposite() const;
  ~Direction();

 private:
  Direction(uint8_t value);
  const uint8_t _value;
};
}  // namespace px::util
#endif  // !DIRECTION_H
