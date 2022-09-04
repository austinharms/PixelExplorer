#include <stdint.h>
#include <unordered_map>
#include <string>

#include "common/RefCount.h"
#include "common/Logger.h"
#include "common/Color.h"
#include "BlockDefinition.h"
#include "BlockFaceDefinition.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKMANIFEST_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKMANIFEST_H_
namespace pixelexplorer::game::block {
	class BlockManifest : public RefCount
	{
	public:
		inline BlockManifest() {
			_blockDefinitions = nullptr;
			_loadedBlocks = false;
			_loadedBlockFaces = false;
		}

		inline virtual ~BlockManifest() {
			unload();
		}

		inline void load() {
			loadBlockFaces();
			loadBlocks();
		}

		inline void unload() {
			unloadBlocks();
			unloadBlockFaces();
		}

		inline void loadBlockFaces() {
			unloadBlockFaces();
			BlockFaceDefinition* face = new BlockFaceDefinition("TEST_FACE", Color(0,255,0,255));
			_blockFaceDefinitions.emplace(face->getName(), face);
			_loadedBlockFaces = true;
			Logger::debug(__FUNCTION__" loaded block faces");
		}

		inline void loadBlocks() {
			unloadBlocks();
			if (!_loadedBlockFaces)
				loadBlockFaces();
			_blockDefinitions = (BlockDefinition**)calloc(1, sizeof(BlockDefinition*));
			BlockFaceDefinition* face = getBlockFace("TEST_FACE");
			BlockFaceDefinition* faces[6];
			for (uint8_t i = 0; i < 6; ++i)
				faces[i] = face;
			_blockDefinitions[0] = new BlockDefinition(faces, 1, "TEST_BLOCK");
			_blockIdMap.emplace(_blockDefinitions[0]->getName(), 1);
			_loadedBlocks = true;
			Logger::debug(__FUNCTION__" loaded blocks");
		}

		inline BlockDefinition* getBlock(uint32_t id) const {
			if (id > _blockIdMap.size() || id == 0) return nullptr;
			return _blockDefinitions[id - 1];
		}

		inline uint32_t getBlockId(const std::string& name) const {
			auto it = _blockIdMap.find(name);
			if (it != _blockIdMap.end()) return it->second;
			return 0;
		}

		inline BlockDefinition* getBlockByName(const std::string& name) {
			return getBlock(getBlockId(name));
		}

		inline BlockFaceDefinition* getBlockFace(const std::string& name) {
			auto it = _blockFaceDefinitions.find(name);
			if (it != _blockFaceDefinitions.end()) return it->second;
			return nullptr;
		}

	private:
		BlockDefinition** _blockDefinitions;
		std::unordered_map<std::string, BlockFaceDefinition*> _blockFaceDefinitions;
		std::unordered_map<std::string, uint32_t> _blockIdMap;
		bool _loadedBlocks;
		bool _loadedBlockFaces;

		inline void unloadBlocks() {
			if (_loadedBlocks) {
				_loadedBlocks = false;
				if (_blockDefinitions != nullptr) {
					for (uint32_t i = 0; i < _blockIdMap.size(); ++i)
						if (_blockDefinitions[i] != nullptr)
							_blockDefinitions[i]->drop();
					free(_blockDefinitions);
					_blockDefinitions = nullptr;
				}

				_blockIdMap.clear();
			}
		}

		inline void unloadBlockFaces() {
			if (_loadedBlockFaces) {
				_loadedBlockFaces = false;
				auto it = _blockFaceDefinitions.begin();
				while (it != _blockFaceDefinitions.end()) {
					if (it->second != nullptr)
						it->second->drop();
					it = _blockFaceDefinitions.erase(it);
				}
			}
		}
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKMANIFEST_H_
