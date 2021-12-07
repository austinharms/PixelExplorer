#include "BaseBlock.h"

#include <iostream>

#include "Package.h"
#include "rendering/Shader.h"
#include "stb_image/stb_image.h"

BaseBlock* BaseBlock::s_blocks = nullptr;
uint32_t BaseBlock::s_blockCount = 0;
std::unordered_map<std::string, uint32_t> BaseBlock::s_blockLookupTable =
std::unordered_map<std::string, uint32_t>();

//BaseBlock::BaseBlock() : RefCounted(false) {}
BaseBlock::BaseBlock() {}

BaseBlock::~BaseBlock() {}

void BaseBlock::UnloadBlocks() {
	if (BaseBlock::s_blocks != nullptr) {
		//for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i)
		//	assert(BaseBlock::s_blocks[i].drop());

		delete[] BaseBlock::s_blocks;
		BaseBlock::s_blocks = nullptr;
	}

	BaseBlock::s_blockCount = 0;
	BaseBlock::s_blockLookupTable.clear();
}

void BaseBlock::LoadBlockManifest() {
	BaseBlock::UnloadBlocks();
	bool manifestChanged = false;
	std::string path = World::GetWorldDir() + "block_manifest";
	std::ifstream manifest(path.c_str(), std::ios::binary);
	if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
		std::cout << "No Block Manifest Found, Creating Empty Manifest"
			<< std::endl;
		BaseBlock::s_blockCount = 0;
		BaseBlock::s_blocks = new BaseBlock[0];
		manifestChanged = true;
	}
	else {
		uint16_t version;
		manifest.read((char*)&version, sizeof(uint16_t));
		assert(version == BaseBlock::MANIFEST_VERSION);

		// Load Block Mapping
		manifest.read((char*)&BaseBlock::s_blockCount,
			sizeof(BaseBlock::s_blockCount));
		std::cout << "Loading " << BaseBlock::s_blockCount
			<< " Blocks from Block Manifest" << std::endl;
		BaseBlock::s_blocks = new BaseBlock[BaseBlock::s_blockCount];
		for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
			uint32_t blockId;
			manifest.read((char*)&blockId, sizeof(uint32_t));
			uint8_t nameLength;
			manifest.read((char*)&nameLength, sizeof(uint8_t));
			char name[256];
			manifest.read(name, nameLength);
			name[nameLength] = '\0';
			std::string blockName = std::string(name);
			BaseBlock::s_blockLookupTable.insert({ blockName, blockId });
			BaseBlock::s_blocks[blockId]._name = name;
			BaseBlock::s_blocks[blockId]._id = blockId;
			BaseBlock::s_blocks[blockId]._loaded = false;
		}
	}

	std::set<std::string> loadedPackages;
	// Load Packages
	uint32_t startingBlockCount = BaseBlock::s_blockCount;
	for (const Package& package : World::GetPackages()) {
		LoadPackage(package, &loadedPackages);
	}

	// Check if we added any new blocks, if so update the manifest
	if (BaseBlock::s_blockCount != startingBlockCount) {
		manifestChanged = true;
		// Change count back to not throw outof bounds when unloading blocks
		BaseBlock::s_blockCount = startingBlockCount;
	}

	manifest.close();
	if (manifestChanged) {
		UpdateManifest();
		std::cout << "Block Manifest Changed, Reloading Manifest" << std::endl;
		LoadBlockManifest();
	}
	else {
		for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
			if (!BaseBlock::s_blocks[i]._loaded) {
				BaseBlock::s_blocks[i]._solid = BaseBlock::s_blocks[0]._solid;
				for (uint8_t j = 0; j < (uint8_t)FaceDirection::FACECOUNT; ++j) {
					s_blocks[i]._faces[j].direction = s_blocks[0]._faces[j].direction;
					s_blocks[i]._faces[j].full = s_blocks[0]._faces[j].full;
					s_blocks[i]._faces[j].indexCount = s_blocks[0]._faces[j].indexCount;
					s_blocks[i]._faces[j].transparent = s_blocks[0]._faces[j].transparent;
					s_blocks[i]._faces[j].vertexCount = s_blocks[0]._faces[j].vertexCount;
					s_blocks[i]._faces[j].vertices = new float[s_blocks[i]._faces[j].vertexCount * 3];
					s_blocks[i]._faces[j].uvs = new float[s_blocks[i]._faces[j].vertexCount * 2];
					s_blocks[i]._faces[j].indices = new uint8_t[s_blocks[i]._faces[j].indexCount];
					memcpy(s_blocks[i]._faces[j].indices, s_blocks[0]._faces[j].indices, s_blocks[i]._faces[j].indexCount * sizeof(uint8_t));
					memcpy(s_blocks[i]._faces[j].vertices, s_blocks[0]._faces[j].vertices, s_blocks[i]._faces[j].vertexCount * 3 * sizeof(float));
					memcpy(s_blocks[i]._faces[j].uvs, s_blocks[0]._faces[j].uvs, s_blocks[i]._faces[j].vertexCount * 2 * sizeof(float));
				}

				BaseBlock::s_blocks[i]._loaded = true;
			}
		}

		CreateTextureAtlas();
	}
}

void BaseBlock::UpdateManifest() {
	std::string path = World::GetWorldDir() + "block_manifest";
	// Version, Block Count,[block id, block name length, block name], Package
	// Count,[package name length, package name]
	std::ofstream manifest(path.c_str(), std::ios::binary | std::ios::trunc);

	// Write Manifest Version
	manifest.write((const char*)&BaseBlock::MANIFEST_VERSION,
		sizeof(BaseBlock::MANIFEST_VERSION));

	// Write Block Count
	uint32_t count = BaseBlock::s_blockLookupTable.size();
	manifest.write((const char*)&count, sizeof(uint32_t));
	// Write Block Loopup Table: id, name length, name
	for (std::pair<const std::string, uint32_t>& pair :
		BaseBlock::s_blockLookupTable) {
		manifest.write((const char*)&pair.second, sizeof(uint32_t));
		uint8_t length = pair.first.length();
		manifest.write((const char*)&length, 1);
		manifest.write(pair.first.c_str(), length);
	}

	// Write null to end for check when reading
	const char nullChar = '\0';
	manifest.write(&nullChar, 1);
	manifest.close();
	std::cout << "Updated Block Manifest" << std::endl;
}

bool BaseBlock::LoadPackage(const Package& pkg,
	std::set<std::string>* loadedPackages) {
	// Dont load package if its already loaded
	if (loadedPackages != nullptr && !loadedPackages->empty() &&
		loadedPackages->find(pkg.getName()) != loadedPackages->end())
		return true;

	if (!pkg.getPackageFound()) return false;
	std::cout << "Loading Block Package: " << pkg.getName() << std::endl;

	// Load Package File
	std::string path = pkg.getPath() + pkg.getName() + ".pxb";
	std::ifstream package(path.c_str(), std::ios::binary);
	if (!package || package.peek() == std::ifstream::traits_type::eof()) {
		package.close();
		std::cout << "Warrning, Failed to Load Block Package: " << pkg.getName()
			<< " Package not Found, Expected Path: " << path << std::endl;
		return false;
	}

	// Get Package Version
	uint16_t version;
	package.read((char*)&version, sizeof(uint16_t));
	if (version != BaseBlock::PACKAGE_VERSION) {
		package.close();
		std::cout << "Warrning, Failed to Load Block Package: " << pkg.getName()
			<< ", Incorrect Package Version" << std::endl;
		return false;
	}

	// Load Package Blocks
	uint16_t blockCount;
	package.read((char*)&blockCount, sizeof(uint16_t));
	for (uint16_t i = 0; i < blockCount; ++i) {
		// Get Block Name and Id
		uint8_t nameLength;
		package.read((char*)&nameLength, sizeof(uint8_t));
		char blockNameChars[256];
		package.read(blockNameChars, nameLength);
		blockNameChars[nameLength] = '\0';
		std::string blockName = pkg.getName() + "/" + std::string(blockNameChars);
		std::unordered_map<std::string, uint32_t>::const_iterator keyPos =
			BaseBlock::s_blockLookupTable.find(blockName);
		bool newBlock = keyPos == BaseBlock::s_blockLookupTable.end();
		uint32_t id;
		if (newBlock) {
			id = BaseBlock::s_blockCount++;
			BaseBlock::s_blockLookupTable.insert({ blockName, id });
			std::cout << "New Block " << blockName << " id: " << id << std::endl;
		}

		if (!newBlock) id = keyPos->second;

		// Load data to BaseBlock
		if (!newBlock) BaseBlock::s_blocks[id]._loaded = true;
		uint8_t solid;
		package.read((char*)&solid, sizeof(uint8_t));
		uint8_t faceCount;

		// Load Block Faces
		package.read((char*)&faceCount, sizeof(uint8_t));
		assert(faceCount == 6);
		for (uint8_t j = 0; j < faceCount; ++j) {
			uint8_t faceInt;
			package.read((char*)&faceInt, sizeof(uint8_t));
			BlockFace* face;
			if (!newBlock) {
				face = &BaseBlock::s_blocks[id]._faces[faceInt];
			}
			else {
				face = new BlockFace();
			}

			face->direction = (FaceDirection)faceInt;

			uint8_t full;
			package.read((char*)&full, sizeof(uint8_t));
			face->full == full == 1;
			uint8_t transparent;
			package.read((char*)&transparent, sizeof(uint8_t));
			face->transparent = transparent == 1;

			uint8_t vertexCount;
			package.read((char*)&vertexCount, sizeof(uint8_t));
			face->vertexCount = vertexCount;
			face->vertices = new float[((uint32_t)vertexCount) * 3];
			face->uvs = new float[((uint32_t)vertexCount) * 2];
			for (uint32_t vert = 0; vert < vertexCount; ++vert) {
				for (uint8_t k = 0; k < 3; ++k) {
					float point;
					package.read((char*)&point, sizeof(float));
					face->vertices[vert * 3 + k] = point;
				}

				for (uint8_t k = 0; k < 2; ++k) {
					float point;
					package.read((char*)&point, sizeof(float));
					face->uvs[vert * 2 + k] = point;
				}
			}

			uint8_t indexCount;
			package.read((char*)&indexCount, sizeof(uint8_t));
			face->indexCount = indexCount;
			face->indices = new uint8_t[indexCount];
			for (uint8_t indice = 0; indice < indexCount; ++indice) {
				uint8_t index;
				package.read((char*)&index, sizeof(uint8_t));
				face->indices[indice] = index;
			}

			if (newBlock) delete face;
		}

		if (!newBlock)
			std::cout << "Loaded Block " << blockName << " id: " << id << std::endl;
	}

	uint8_t end;
	package.read((char*)&end, sizeof(uint8_t));
	loadedPackages->insert(pkg.getName());

	if (end != 0) {
		std::cout << "Warrning May Have Failed Loading Package, No Trailing NULL"
			<< std::endl;
	}

	package.close();
	return true;
}

void BaseBlock::CreateTextureAtlas() {
	std::cout << "Creating Texture Atlas" << std::endl;
	uint32_t textureCount = 0;
	uint8_t* foundTextures = new uint8_t[BaseBlock::s_blockCount];
	std::string sides[7] = { std::string(""),std::string("_front"), std::string("_back"), std::string("_left"), std::string("_right"), std::string("_top"), std::string("_bottom") };
	uint8_t sideFlags[7] = { 0b00000001,0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b01000000 };
	memset(foundTextures, 0, BaseBlock::s_blockCount);
	for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
		size_t splitIndex = BaseBlock::s_blocks[i]._name.find("/");
		std::string packageName = BaseBlock::s_blocks[i]._name.substr(0, splitIndex);
		const Package* pkg = World::GetPackage(packageName);
		if (pkg == nullptr || !pkg->getPackageFound()) continue;
		std::string texturePath = pkg->getPath() + "textures\\";
		texturePath += BaseBlock::s_blocks[i]._name.substr(splitIndex + 1, BaseBlock::s_blocks[i]._name.length() - splitIndex);

		int32_t width;
		int32_t height;
		int32_t channelCount;
		uint8_t* img = nullptr;

		for (uint8_t j = 0; j < 7; ++j) {
			img = stbi_load((texturePath + sides[j] + ".png").c_str(), &width, &height, &channelCount, 4);
			stbi_image_free(img);
			if (width == 25 && height == 25 && img != nullptr)
			{
				foundTextures[i] |= sideFlags[j];
				foundTextures[i] |= 0b10000000;
				++textureCount;
			}
		}
		//Check that default texture loaded
		assert(i == 0 && textureCount >= 1 || i > 0);
	}

	uint16_t x = 0;
	uint16_t y = 0;
	uint32_t loadedTextures = 0;
	uint16_t atlasSize = ceil(sqrt(textureCount));
	uint32_t atlasPixelSize = 25 * atlasSize;
	uint32_t atlasByteWidth = 25 * atlasSize * 4;
	uint64_t atlasByteSize = (uint64_t)atlasPixelSize * atlasPixelSize * 4;
	uint8_t* textureAtlas = (uint8_t*)malloc(atlasByteSize);
	assert(textureAtlas != nullptr);
	memset(textureAtlas, 255, atlasByteSize);
	float tileSize = (1.0f / atlasPixelSize) * 25;

	for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
		if (foundTextures[i] & 0b10000000) {
			size_t splitIndex = BaseBlock::s_blocks[i]._name.find("/");
			std::string packageName = BaseBlock::s_blocks[i]._name.substr(0, splitIndex);
			const Package* pkg = World::GetPackage(packageName);
			if (pkg == nullptr || !pkg->getPackageFound()) continue;
			std::string texturePath = pkg->getPath() + "textures\\";
			texturePath += BaseBlock::s_blocks[i]._name.substr(splitIndex + 1, BaseBlock::s_blocks[i]._name.length() - splitIndex);

			int32_t width;
			int32_t height;
			int32_t channelCount;
			uint8_t* img = nullptr;
			uint16_t defaultX = 0;
			uint16_t defaultY = 0;

			for (uint8_t j = 0; j < 7; ++j) {
				uint16_t currentX = defaultX;
				uint16_t currentY = defaultY;
				if (foundTextures[i] & sideFlags[j]) {
					img = stbi_load((texturePath + sides[j] + ".png").c_str(), &width, &height, &channelCount, 4);
					if (width == 25 && height == 25 && img != nullptr) {
						++loadedTextures;
						for (uint32_t copyHeight = 0; copyHeight < 25; ++copyHeight)
							memcpy(textureAtlas + (x * 25 * 4) + (atlasByteWidth * y * 25) + (atlasByteWidth * copyHeight), img + ((24 - copyHeight) * 25 * 4), 25 * 4);

						if (j == 0) {
							defaultX = x;
							defaultY = y;
						}

						currentX = x;
						currentY = y;

						++x;
						if (x >= atlasSize) {
							x = 0;
							++y;
						}
					}
					else
					{
						std::cout << "Warrning failed to load block texture " << texturePath << sides[j] << ".png" << std::endl;
					}
					stbi_image_free(img);
				}

				if (j != 0) {
					BlockFace* face = &BaseBlock::s_blocks[i]._faces[j - 1];
					for (uint8_t k = 0; k < face->vertexCount; ++k) {
						face->uvs[k * 2] = (currentX * tileSize) + (tileSize * face->uvs[k * 2]);
						face->uvs[k * 2 + 1] = (currentY * tileSize) + (tileSize * face->uvs[k * 2 + 1]);
					}
				}
			}
		}
		else {
			std::cout << "Warrning failed to find texture for block " << BaseBlock::s_blocks[i]._name << std::endl;

			for (uint8_t j = 0; j < 6; ++j) {
				BlockFace* face = &BaseBlock::s_blocks[i]._faces[j];
				for (uint8_t k = 0; k < face->vertexCount * 2; ++k)
					face->uvs[k] = tileSize * face->uvs[k];
			}
		}
	}

	std::cout << "Found " << textureCount << " Textures, Loaded " << loadedTextures << std::endl;
	Chunk::SetChunkMaterialTexture(textureAtlas, atlasPixelSize, atlasPixelSize);
	free(textureAtlas);
	delete[] foundTextures;
}
