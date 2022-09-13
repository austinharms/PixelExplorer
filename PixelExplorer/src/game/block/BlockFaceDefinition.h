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
	class BlockFaceDefinition : public RefCount
	{
	public:
		inline BlockFaceDefinition(std::filesystem::path definitionFilePath) : _name(definitionFilePath.stem().string()) {
			std::ifstream stream(definitionFilePath);
			std::string line;
			while (getline(stream, line))
			{
				if (line.find("color=#") == 0 && line.find(";") == 15) {
					_color.setColor(std::stoi(line.c_str() + 7, nullptr, 16));
				}
				else if (!line.empty()) {
					Logger::warn(__FUNCTION__" malformed line \"" + line + "\" in block face definition " + _name);
				}
			}

			Logger::debug(__FUNCTION__" loaded block face definition " + _name);
		}

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
