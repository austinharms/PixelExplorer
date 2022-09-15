#include <stdint.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <string>

#include "common/RefCount.h"
#include "common/Color.h"
#include "common/Logger.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKFACEDEFINITION_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKFACEDEFINITION_H_
namespace pixelexplorer::game::block {
	struct BlockFaceDefinition : public RefCount
	{
	public:
		const std::string Name;
		pixelexplorer::Color Color;

		inline BlockFaceDefinition(const std::string name, const pixelexplorer::Color color) : Name(name), Color(color) {}

		inline virtual ~BlockFaceDefinition() {}
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKDEFINITION_H_
