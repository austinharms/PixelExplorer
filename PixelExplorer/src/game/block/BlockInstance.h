#include <stdint.h>

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKINSTANCE_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKINSTANCE_H_
namespace pixelexplorer::game::block {
	class BlockInstance
	{
	public:
		inline uint32_t getId() const { return _value & IDMASK; }
		inline uint8_t getOther() const { return _value >> 24; }

	private:
		static const uint32_t IDMASK = 0x00ffffff;
		static const uint32_t OTHERMASK = 0xff000000;
		uint32_t _value;
	};
}
#endif