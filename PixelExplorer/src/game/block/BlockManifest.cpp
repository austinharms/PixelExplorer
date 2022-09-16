#include "BlockManifest.h"

#include <forward_list>

namespace pixelexplorer::game::block {
	BlockManifest::BlockManifest(const std::filesystem::path& externalResourcePath) : _resourcePath(externalResourcePath) {
		_blockDefinitions = nullptr;
		_loadedBlocks = false;
		_loadedBlockFaces = false;
		_lastBlockId = 0;
		loadBlockIdMap();
		loadBlockFaces();
		loadBlocks();
	}

	BlockManifest::~BlockManifest() {
		unloadBlocks();
		unloadBlockFaces();
		unloadBlockIdMap();
	}

	void BlockManifest::loadBlockFaces() {
		unloadBlockFaces();
		std::filesystem::path blockFacePath(OSHelper::getAssetPath(std::filesystem::path("blocks") / "faces"));
		for (const auto& entry : std::filesystem::directory_iterator(blockFacePath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pxface") {
				BlockFaceDefinition* face = parseBlockFaceFile(entry);
				if (face) _blockFaceDefinitions.emplace(face->Name, face);
			}
		}

		_loadedBlockFaces = true;
		Logger::debug(__FUNCTION__" loaded all Block Face Definitions");
	}

	BlockFaceDefinition* BlockManifest::parseBlockFaceFile(const std::filesystem::path& path)
	{
		if (path.stem().string() == "NONE") {
			Logger::warn(__FUNCTION__" found pxface file with reserved name NONE, file not loaded");
			return nullptr;
		}

		bool hasColor = false;
		Color faceColor;
		std::ifstream stream(path);
		std::string line;
		while (getline(stream, line))
		{
			// allow comments using ;
			if (line.rfind(";", 0) == 0) continue;
			if (line.rfind("color=#", 0) == 0 && line.find(";") == 15) {
				if (hasColor) {
					Logger::warn(__FUNCTION__" color was reassigned in pxface file " + path.string() + " using original color");
					continue;
				}

				faceColor.setColor(std::stoul(line.c_str() + 7, nullptr, 16));
				hasColor = true;
			}
			else if (!line.empty()) {
				Logger::warn(__FUNCTION__" malformed line \"" + line + "\" in pxface file " + path.string() + " file not loaded");
				return nullptr;
			}
		}

		if (!hasColor) {
			Logger::warn(__FUNCTION__" missing color property (color=#FFFFFFFF; or any hex color with alpha) pxface file " + path.string() + " file not loaded");
			return nullptr;
		}

		BlockFaceDefinition* faceDef = new BlockFaceDefinition(path.stem().string(), faceColor);
		Logger::debug(__FUNCTION__" loaded Block Face Definition \"" + faceDef->Name + "\" from pxface file " + path.string());
		return faceDef;
	}

	BlockDefinition* BlockManifest::parseBlockFile(const std::filesystem::path& path)
	{
		bool hasFaces[6];
		BlockFaceDefinition* faces[6];
		memset(hasFaces, 0, sizeof(hasFaces));
		memset(faces, 0, sizeof(faces));

		std::ifstream stream(path);
		std::string line;
		while (getline(stream, line))
		{
			// allow comments using ;
			if (line.rfind(";", 0) == 0) continue;
			bool foundFace = false;
			std::string faceName;
			if (line.rfind("frontFace=", 0) == 0 && line.find(";") > 11) {
				if (hasFaces[(uint32_t)FaceDirection::FRONT]) {
					Logger::warn(__FUNCTION__" frontFace was reassigned in pxblock file " + path.string() + " using original face");
					continue;
				}

				hasFaces[(uint32_t)FaceDirection::FRONT] = true;
				faceName = line.substr(10, line.find(";") - 10);
				faces[(uint32_t)FaceDirection::FRONT] = getBlockFace(faceName, &foundFace);
			}
			else if (line.rfind("backFace=", 0) == 0 && line.find(";") > 10) {
				if (hasFaces[(uint32_t)FaceDirection::BACK]) {
					Logger::warn(__FUNCTION__" backFace was reassigned in pxblock file " + path.string() + " using original face");
					continue;
				}

				hasFaces[(uint32_t)FaceDirection::BACK] = true;
				faceName = line.substr(9, line.find(";") - 9);
				faces[(uint32_t)FaceDirection::BACK] = getBlockFace(faceName, &foundFace);
			}
			else if (line.rfind("leftFace=", 0) == 0 && line.find(";") > 10) {
				if (hasFaces[(uint32_t)FaceDirection::LEFT]) {
					Logger::warn(__FUNCTION__" leftFace was reassigned in pxblock file " + path.string() + " using original face");
					continue;
				}

				hasFaces[(uint32_t)FaceDirection::LEFT] = true;
				faceName = line.substr(9, line.find(";") - 9);
				faces[(uint32_t)FaceDirection::LEFT] = getBlockFace(faceName, &foundFace);
			}
			else if (line.rfind("rightFace=", 0) == 0 && line.find(";") > 11) {
				if (hasFaces[(uint32_t)FaceDirection::RIGHT]) {
					Logger::warn(__FUNCTION__" rightFace was reassigned in pxblock file " + path.string() + " using original face");
					continue;
				}

				hasFaces[(uint32_t)FaceDirection::RIGHT] = true;
				faceName = line.substr(10, line.find(";") - 10);
				faces[(uint32_t)FaceDirection::RIGHT] = getBlockFace(faceName, &foundFace);
			}
			else if (line.rfind("topFace=", 0) == 0 && line.find(";") > 9) {
				if (hasFaces[(uint32_t)FaceDirection::TOP]) {
					Logger::warn(__FUNCTION__" topFace was reassigned in pxblock file " + path.string() + " using original face");
					continue;
				}

				hasFaces[(uint32_t)FaceDirection::TOP] = true;
				faceName = line.substr(8, line.find(";") - 8);
				faces[(uint32_t)FaceDirection::TOP] = getBlockFace(faceName, &foundFace);
			} else if (line.rfind("bottomFace=", 0) == 0 && line.find(";") > 12) {
				if (hasFaces[(uint32_t)FaceDirection::BOTTOM]) {
					Logger::warn(__FUNCTION__" bottomFace was reassigned in pxblock file " + path.string() + " using original face");
					continue;
				}

				hasFaces[(uint32_t)FaceDirection::BOTTOM] = true;
				faceName = line.substr(11, line.find(";") - 11);
				faces[(uint32_t)FaceDirection::BOTTOM] = getBlockFace(faceName, &foundFace);
			}
			else if (!line.empty()) {
				Logger::warn(__FUNCTION__" malformed line \"" + line + "\" in pxblock file " + path.string() + " file not loaded");
				return nullptr;
			}

			if (!foundFace)
				Logger::warn(__FUNCTION__" failed to find block face \"" + faceName + "\", created default face");
		}

		if (!hasFaces[(uint32_t)FaceDirection::FRONT]) {
			Logger::warn(__FUNCTION__" missing frontFace property in pxblock file " + path.string() + " file not loaded");
			return nullptr;
		}

		if (!hasFaces[(uint32_t)FaceDirection::BACK]) {
			Logger::warn(__FUNCTION__" missing backFace property in pxblock file " + path.string() + " file not loaded");
			return nullptr;
		}

		if (!hasFaces[(uint32_t)FaceDirection::LEFT]) {
			Logger::warn(__FUNCTION__" missing leftFace property in pxblock file " + path.string() + " file not loaded");
			return nullptr;
		}

		if (!hasFaces[(uint32_t)FaceDirection::RIGHT]) {
			Logger::warn(__FUNCTION__" missing rightFace property in pxblock file " + path.string() + " file not loaded");
			return nullptr;
		}

		if (!hasFaces[(uint32_t)FaceDirection::TOP]) {
			Logger::warn(__FUNCTION__" missing topFace property in pxblock file " + path.string() + " file not loaded");
			return nullptr;
		}

		if (!hasFaces[(uint32_t)FaceDirection::BOTTOM]) {
			Logger::warn(__FUNCTION__" missing bottomFace property in pxblock file " + path.string() + " file not loaded");
			return nullptr;
		}

		uint32_t id = getBlockId(path.stem().string());
		if (id == 0) {
			id = ++_lastBlockId;
			Logger::debug(__FUNCTION__" found new block definition \"" + path.stem().string() + "\" Assigning Id: " + std::to_string(id));
		}

		BlockDefinition* blockDef = new BlockDefinition(faces, id, path.stem().string());
		Logger::debug(__FUNCTION__" loaded Block Definition \"" + blockDef->getName() + "\" with id " + std::to_string(blockDef->getId()) + " from pxblock file " + path.string());
		return blockDef;
	}

	BlockDefinition* BlockManifest::createMissingBlock(const std::string& name, uint32_t id)
	{
		// we are assuming PX_MISSING_FACE does not exist and will be created with default parameters
		BlockFaceDefinition* faceDef = getBlockFace("PX_MISSING_FACE");
		BlockFaceDefinition* faces[6] = { faceDef, faceDef, faceDef, faceDef, faceDef, faceDef };
		return new BlockDefinition(faces, id, name);
	}

	void BlockManifest::loadBlocks() {
		unloadBlocks();
		uint32_t allocatedBlockCount = _lastBlockId;
		std::forward_list<BlockDefinition*> newBlockList;
		if (_lastBlockId != 0) {
			_blockDefinitions = (BlockDefinition**)calloc(_lastBlockId, sizeof(BlockDefinition*));
			if (_blockDefinitions == nullptr) Logger::fatal(__FUNCTION__" failed to allocate Block Definitions Array");
		}

		std::filesystem::path blockPath(OSHelper::getAssetPath("blocks"));
		for (const auto& entry : std::filesystem::directory_iterator(blockPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pxblock") {
				BlockDefinition* block = parseBlockFile(entry);
				if (block) {
					if (block->getId() <= allocatedBlockCount) {
						_blockDefinitions[block->getId()] = block;
					}
					else {
						newBlockList.emplace_front(block);
						_blockIdMap.emplace(block->getName(), block->getId());
					}
				}
			}
		}

		for (uint32_t i = 0; i < allocatedBlockCount; ++i) {
			if (_blockDefinitions[i] == nullptr) {
				std::string name;
				for (auto it = _blockIdMap.begin(); it != _blockIdMap.end(); ++it) {
					if (it->second == i + 1) {
						name = it->first;
						break;
					}
				}

				Logger::warn(__FUNCTION__" missing block definition for block \"" + name + "\", creating default block");
				_blockDefinitions[i] = createMissingBlock(name, i + 1);
			}
		}

		if (allocatedBlockCount != _lastBlockId) {
			Logger::debug(__FUNCTION__" new Block Definitions loaded resizing block array");
			_blockDefinitions = (BlockDefinition**)realloc(_blockDefinitions, _lastBlockId * sizeof(BlockDefinition*));
			if (_blockDefinitions == nullptr) Logger::fatal(__FUNCTION__" failed to reallocate Block Definitions Array");
			for (uint32_t i = _lastBlockId; i > allocatedBlockCount; --i) {
				_blockDefinitions[i - 1] = newBlockList.front();
				newBlockList.pop_front();
			}

			assert(newBlockList.empty());
		}

		_loadedBlocks = true;
		Logger::debug(__FUNCTION__" loaded all Block Definitions");
		//_blockDefinitions = (BlockDefinition**)calloc(1, sizeof(BlockDefinition*));
		//BlockFaceDefinition* face = getBlockFace("GREEN_FACE");
		//BlockFaceDefinition* faces[6];
		//for (uint8_t i = 0; i < 6; ++i)
		//	faces[i] = face;
		//_blockDefinitions[0] = new BlockDefinition(faces, 1, "TEST_BLOCK");
		//_blockIdMap.emplace(_blockDefinitions[0]->getName(), 1);
		//_loadedBlocks = true;
		//Logger::debug(__FUNCTION__" loaded blocks");
	}

	BlockDefinition* BlockManifest::getBlock(uint32_t id) const {
		if (id > _blockIdMap.size() || id == 0) return nullptr;
		return _blockDefinitions[id - 1];
	}

	uint32_t BlockManifest::getBlockId(const std::string& name) const {
		auto it = _blockIdMap.find(name);
		if (it != _blockIdMap.end()) return it->second;
		return 0;
	}

	BlockDefinition* BlockManifest::getBlockByName(const std::string& name) {
		return getBlock(getBlockId(name));
	}

	BlockFaceDefinition* BlockManifest::getBlockFace(const std::string& name, bool* exist) {
		if (name == "NONE") return nullptr;
		auto it = _blockFaceDefinitions.find(name);
		if (it == _blockFaceDefinitions.end()) return createDefaultBlockFace(name);
		if (exist) *exist = true;
		return it->second;
	}

	void BlockManifest::unloadBlocks() {
		if (_loadedBlocks) {
			_loadedBlocks = false;
			if (_blockDefinitions != nullptr) {
				for (uint32_t i = 0; i < _lastBlockId; ++i) {
					_blockDefinitions[i]->drop();
					_blockDefinitions[i] = nullptr;
				}

				free(_blockDefinitions);
				_blockDefinitions = nullptr;
			}
		}
	}

	void BlockManifest::loadBlockIdMap()
	{
		_lastBlockId = 0;
		if (_resourcePath == "") return;
	}

	void BlockManifest::unloadBlockFaces() {
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

	void BlockManifest::unloadBlockIdMap()
	{
		_blockIdMap.clear();
		_lastBlockId = 0;
		if (_resourcePath == "") return;
	}

	BlockFaceDefinition* BlockManifest::createDefaultBlockFace(const std::string& name) {
		if (name == "NONE") {
			// this should never be called with name NONE
			Logger::error(__FUNCTION__" attempted to create Block Face Definition with reserved name NONE");
			return nullptr;
		}

		Logger::debug(__FUNCTION__" created default block face with name " + name);
		BlockFaceDefinition* newDef = new BlockFaceDefinition(name, Color(255, 0, 255, 255));
		_blockFaceDefinitions.emplace(name, newDef);
		return newDef;
	}
}