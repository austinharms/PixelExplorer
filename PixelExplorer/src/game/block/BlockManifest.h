#include <stdint.h>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <forward_list>

#include "common/RefCount.h"
#include "BlockDefinition.h"
#include "BlockFaceDefinition.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKMANIFEST_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKMANIFEST_H_
namespace pixelexplorer::game::block {
	class BlockManifest : public RefCount
	{
	public:
		// externalResourcePath is used to save the block id mapping and it will also attempt to load pxblock and pxface files
		// if externalResourcePath is empty the block id map will not be loaded/saved
		BlockManifest(const std::filesystem::path& externalResourcePath);
		virtual ~BlockManifest();
		BlockDefinition* getBlock(uint32_t id) const;
		uint32_t getBlockId(const std::string& name) const;
		BlockDefinition* getBlockByName(const std::string& name);
		// will *always return a BlockFaceDefinition if a definition does not exist with the supplied name a new definition is created with default parameters, it will also set exist to true if it is not null and the definition was found, *this will not return a definition if allocation fails
		BlockFaceDefinition* getBlockFace(const std::string& name, bool* exist = nullptr);

	private:
		BlockDefinition** _blockDefinitions;
		std::unordered_map<std::string, BlockFaceDefinition*> _blockFaceDefinitions;
		std::unordered_map<std::string, uint32_t> _blockIdMap;
		const std::filesystem::path _resourcePath;
		uint32_t _lastBlockId;

		void loadBlockFaces(const std::filesystem::path& faceFolderPath);
		void loadBlockIdMap();
		void allocateBlockArray(std::forward_list<BlockDefinition*>& newBlockList, uint32_t& allocatedBlockCount);
		void loadBlocks(const std::filesystem::path& blockFolderPath, uint32_t allocatedBlockCount, std::forward_list<BlockDefinition*>& newBlockList);
		void createMissingBlocks();
		void unloadBlocks();
		void unloadBlockFaces();
		void unloadBlockIdMap();
		void saveBlockIdMap();
		BlockFaceDefinition* createDefaultBlockFace(const std::string& name);
		BlockFaceDefinition* parseBlockFaceFile(const std::filesystem::path& path);
		BlockDefinition* parseBlockFile(const std::filesystem::path& path);
		BlockDefinition* createMissingBlock(const std::string& name, uint32_t id);
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKMANIFEST_H_
