#include <stdint.h>
#include <string>

#include "RefCount.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
namespace pixelexplorer::game::block {
	class BlockFaceDefinition : public RefCount
	{
	public:
		inline BlockFaceDefinition(const std::string name, uint32_t color) : _name(name), _color(color) {}

		inline virtual ~BlockFaceDefinition() {}

		const uint32_t getColor() const { return _color; }

		const std::string getName() const { return _name; }

	private:
		const std::string _name;
		uint32_t _color;
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
