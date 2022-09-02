#include <stdint.h>
#include <string>

#include "RefCount.h"
#include "Color.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKFACEDEFINITION_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKFACEDEFINITION_H_
namespace pixelexplorer::game::block {
	class BlockFaceDefinition : public RefCount
	{
	public:
		inline BlockFaceDefinition(const std::string name, const Color color) : _name(name), _color(color) {}

		inline virtual ~BlockFaceDefinition() {}

		const Color getColor() const { return _color; }

		const std::string getName() const { return _name; }

	private:
		const std::string _name;
		Color _color;

	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
