#include "BaseBlock.h"

#include <iostream>

#include "Package.h"
#include "rendering/Shader.h"
#include "stb_image/stb_image.h"

BaseBlock* BaseBlock::s_blocks = nullptr;
uint32_t BaseBlock::s_blockCount = 0;
std::unordered_map<std::string, uint32_t> BaseBlock::s_blockLookupTable =
    std::unordered_map<std::string, uint32_t>();

BaseBlock::BaseBlock() : RefCounted(false) {}

BaseBlock::~BaseBlock() {}

void BaseBlock::UnloadBlocks() {
  if (BaseBlock::s_blocks != nullptr) {
    for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i)
      assert(BaseBlock::s_blocks[i].drop());

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
  } else {
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
      BaseBlock::s_blockLookupTable.insert({blockName, blockId});
      BaseBlock::s_blocks[blockId]._name = blockName;
      BaseBlock::s_blocks[blockId]._id = blockId;
      BaseBlock::s_blocks[blockId]._loaded = false;
    }
  }

  std::set<std::string> loadedPackages;
  // Load Packages
  uint32_t startingBlockCount = BaseBlock::s_blockCount;
  for (const Package& package : World::GetPackages())
    LoadPackage(package, &loadedPackages);

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
  } else {
    for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
      if (!BaseBlock::s_blocks[i]._loaded) {
        BaseBlock::s_blocks[i]._solid = BaseBlock::s_blocks[0]._solid;
        memcpy(&BaseBlock::s_blocks[i]._faces, &BaseBlock::s_blocks[0]._faces,
               sizeof(BaseBlock::s_blocks[0]._faces));
        for (uint8_t j = 0; j < (uint8_t)FaceDirection::FACECOUNT; ++j) {
          BaseBlock::s_blocks[i]._faces[j].vertices =
              new float[BaseBlock::s_blocks[i]._faces[j].vertexCount * 3];
          BaseBlock::s_blocks[i]._faces[j].uvs =
              new float[BaseBlock::s_blocks[i]._faces[j].vertexCount * 2];
          BaseBlock::s_blocks[i]._faces[j].indices =
              new uint8_t[BaseBlock::s_blocks[i]._faces[j].indexCount];
          memcpy(
              &BaseBlock::s_blocks[i]._faces[j].vertices,
              &BaseBlock::s_blocks[0]._faces[j].vertices,
              BaseBlock::s_blocks[i]._faces[j].vertexCount * 3 * sizeof(float));
          memcpy(
              &BaseBlock::s_blocks[i]._faces[j].uvs,
              &BaseBlock::s_blocks[0]._faces[j].uvs,
              BaseBlock::s_blocks[i]._faces[j].vertexCount * 2 * sizeof(float));
          memcpy(
              &BaseBlock::s_blocks[i]._faces[j].indices,
              &BaseBlock::s_blocks[0]._faces[j].indices,
              BaseBlock::s_blocks[i]._faces[j].indexCount * sizeof(uint8_t));
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
      BaseBlock::s_blockLookupTable.insert({blockName, id});
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
      } else {
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
  uint32_t textureCount = 0;
  uint8_t* foundTextures = new uint8_t[BaseBlock::s_blockCount];
  for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
    
  }
    
    int32_t width;
  int32_t height;
  int32_t channelCount;
  uint8_t* img = stbi_load(filename, &width, &height, &channelCount, 4);
  stbi_image_free(img);
}
