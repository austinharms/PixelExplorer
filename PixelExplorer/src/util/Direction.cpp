#include "Direction.h"
namespace px::util {
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

	Direction Direction::fromIndex(int32_t index)
	{
		switch (index) {
		case 0:  // FRONT
			return FRONT;

		case 1:  // BACK
			return BACK;

		case 2:  // LEFT
			return LEFT;

		case 3:  // RIGHT
			return RIGHT;

		case 4:  // TOP
			return TOP;

		case 5:  // BOTTOM
			return BOTTOM;
		}

		return NONE;
	}

	Direction::operator uint8_t() const { return _value; }

	bool Direction::operator==(const Direction& d) const {
		return d._value == _value;
	}

	bool Direction::operator!=(const Direction& d) const {
		return d._value != _value;
	}

	Direction Direction::operator-(const Direction& d) const {
		return getOpposite();
	}

	Direction Direction::operator=(const uint8_t val) const {
		return Direction(val);
	}

	Direction::operator glm::vec3() const { return toVec3(); }

	Direction::operator glm::ivec3() const
	{
		return toIVec3();
	}

	glm::vec3 Direction::toVec3() const {
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

	glm::ivec3 Direction::toIVec3() const
	{
		switch (_value) {
		case 0b00000001:  // FRONT
			return glm::ivec3(0, 0, 1);

		case 0b00000010:  // BACK
			return glm::ivec3(0, 0, -1);

		case 0b00000100:  // LEFT
			return glm::ivec3(-1, 0, 0);

		case 0b00001000:  // RIGHT
			return glm::ivec3(1, 0, 0);

		case 0b00010000:  // TOP
			return glm::ivec3(0, 1, 0);

		case 0b00100000:  // BOTTOM
			return glm::ivec3(0, -1, 0);

		case 0b00111111:  // ALL
			return glm::ivec3(1, 1, 1);

		case 0b00000000:  // NONE
			return glm::ivec3(0, 0, 0);
		}
	}

	int32_t Direction::toIndex() const
	{
		switch (_value) {
		case 0b00000001:  // FRONT
			return 0;

		case 0b00000010:  // BACK
			return 1;

		case 0b00000100:  // LEFT
			return 2;

		case 0b00001000:  // RIGHT
			return 3;

		case 0b00010000:  // TOP
			return 4;

		case 0b00100000:  // BOTTOM
			return 5;
		}

		return -1;
	}

	Direction Direction::getOpposite() const {
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
}  // namespace px::util