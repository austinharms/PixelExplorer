#include "BlockManifest.h"

#include "common/Logger.h"
#include "common/Color.h"
#include "common/OSHelpers.h"

namespace pixelexplorer::game::block {
	BlockManifest::BlockManifest(const std::filesystem::path& externalResourcePath) : _resourcePath(externalResourcePath) {
		_blockDefinitions = nullptr;
		_lastBlockId = 0;

		std::filesystem::path externalBlockFacePath = externalResourcePath / "blocks" / "faces";
		std::filesystem::create_directories(externalBlockFacePath);
		loadBlockFaces(externalBlockFacePath);
		loadBlockFaces(OSHelper::getAssetPath(std::filesystem::path("blocks") / "faces"));

		loadBlockIdMap();

		std::filesystem::path externalBlockPath = externalResourcePath / "blocks";
		std::filesystem::create_directories(externalBlockFacePath);
		std::forward_list<BlockDefinition*> newBlocks;
		uint32_t allocatedBlockCount = 0;
		allocateBlockArray(newBlocks, allocatedBlockCount);
		loadBlocks(externalBlockPath, allocatedBlockCount, newBlocks);
		loadBlocks(OSHelper::getAssetPath("blocks"), allocatedBlockCount, newBlocks);
		allocateBlockArray(newBlocks, allocatedBlockCount);
		createMissingBlocks();
		saveBlockIdMap();
	}

	BlockManifest::~BlockManifest() {
		unloadBlocks();
		unloadBlockFaces();
		unloadBlockIdMap();
	}

	void BlockManifest::loadBlockFaces(const std::filesystem::path& faceFolderPath) {
		for (const auto& entry : std::filesystem::directory_iterator(faceFolderPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pxface") {
				BlockFaceDefinition* face = parseBlockFaceFile(entry);
				if (face) {
					if (!_blockFaceDefinitions.emplace(face->Name, face).second) {
						Logger::warn(__FUNCTION__" Face with name \"" + face->Name + "\" already loaded, new Face not loaded");
						face->drop();
					}
				}
			}
		}
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
				stream.close();
				return nullptr;
			}
		}

		stream.close();
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
			}
			else if (line.rfind("bottomFace=", 0) == 0 && line.find(";") > 12) {
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

	void BlockManifest::loadBlocks(const std::filesystem::path& blockFolderPath, uint32_t allocatedBlockCount, std::forward_list<BlockDefinition*>& newBlockList) {
		for (const auto& entry : std::filesystem::directory_iterator(blockFolderPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".pxblock") {
				BlockDefinition* block = parseBlockFile(entry);
				if (block) {
					if (block->getId() <= allocatedBlockCount) {
						if (_blockDefinitions[block->getId() - 1] == nullptr) {
							_blockDefinitions[block->getId() - 1] = block;
						}
						else {
							Logger::warn(__FUNCTION__" Block with name \"" + block->getName() + "\" already loaded, new Block not loaded");
							block->drop();
						}
					}
					else {
						if (_blockIdMap.emplace(block->getName(), block->getId()).second) {
							newBlockList.emplace_front(block);
						}
						else {
							Logger::warn(__FUNCTION__" Block with name \"" + block->getName() + "\" already loaded, new Block not loaded");
							block->drop();
						}
					}
				}
			}
		}
	}

	void BlockManifest::createMissingBlocks()
	{
		for (uint32_t i = 0; i < _lastBlockId; ++i) {
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
		if (name == "NONE") {
			if (exist) *exist = true;
			return nullptr;
		}

		auto it = _blockFaceDefinitions.find(name);
		if (it == _blockFaceDefinitions.end()) return createDefaultBlockFace(name);
		if (exist) *exist = true;
		return it->second;
	}

	void BlockManifest::unloadBlocks() {
		if (_blockDefinitions != nullptr) {
			for (uint32_t i = 0; i < _lastBlockId; ++i) {
				_blockDefinitions[i]->drop();
				_blockDefinitions[i] = nullptr;
			}

			free(_blockDefinitions);
			_blockDefinitions = nullptr;
		}
	}

	void BlockManifest::loadBlockIdMap()
	{
		_lastBlockId = 0;
		if (_resourcePath == "") return;
		std::ifstream stream(_resourcePath / "blocks.map");
		std::string line;
		while (getline(stream, line)) {
			if (line.length() < sizeof(uint32_t) / sizeof(char) + sizeof(char)) {
				Logger::warn(__FUNCTION__" found invalid line in block id map file " + (_resourcePath / "blocks.map").string());
				continue;
			}

			_blockIdMap.emplace(line.substr(sizeof(uint32_t) / sizeof(char)), ((uint32_t*)line.c_str())[0]);
		}

		_lastBlockId = _blockIdMap.size();
		stream.close();
	}

	void BlockManifest::allocateBlockArray(std::forward_list<BlockDefinition*>& newBlockList, uint32_t& allocatedBlockCount)
	{
		if (allocatedBlockCount != _lastBlockId) {
			if (_blockDefinitions == nullptr) {
				_blockDefinitions = (BlockDefinition**)calloc(_lastBlockId, sizeof(BlockDefinition*));
			}
			else {
				_blockDefinitions = (BlockDefinition**)realloc(_blockDefinitions, _lastBlockId * sizeof(BlockDefinition*));
			}

			if (_blockDefinitions == nullptr) Logger::fatal(__FUNCTION__" failed to allocate Block Definitions array");
			if (!newBlockList.empty()) {
				for (uint32_t i = _lastBlockId; i > allocatedBlockCount; --i) {
					_blockDefinitions[i - 1] = newBlockList.front();
					newBlockList.pop_front();
				}
			}

			allocatedBlockCount = _lastBlockId;
			assert(newBlockList.empty());
		}
	}

	void BlockManifest::unloadBlockFaces() {
		auto it = _blockFaceDefinitions.begin();
		while (it != _blockFaceDefinitions.end()) {
			if (it->second != nullptr)
				it->second->drop();
			it = _blockFaceDefinitions.erase(it);
		}
	}

	void BlockManifest::unloadBlockIdMap()
	{
		_blockIdMap.clear();
		_lastBlockId = 0;
	}

	void BlockManifest::saveBlockIdMap()
	{
		if (_resourcePath == "") return;
		std::ofstream stream(_resourcePath / "blocks.map");
		for (auto& idMapping : _blockIdMap) {
			uint32_t id = idMapping.second;
			stream.write((const char*)&(idMapping.second), sizeof(uint32_t));
			stream.write(idMapping.first.c_str(), idMapping.first.size());
			stream << std::endl;
		}

		stream.close();
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