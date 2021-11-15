#include "BaseBlock.h"

#include <iostream>

BaseBlock* BaseBlock::s_blocks = nullptr;
uint32_t BaseBlock::s_blockCount = 0;
std::set<std::string> BaseBlock::s_packages = std::set<std::string>();
std::unordered_map<std::string, uint32_t> BaseBlock::s_blockLookupTable =
    std::unordered_map<std::string, uint32_t>();

BaseBlock::BaseBlock() {}

BaseBlock::~BaseBlock() {}

void BaseBlock::UnloadBlocks() { 
  if (BaseBlock::s_blocks != nullptr) {
    for (uint32_t i = 0; i <= BaseBlock::s_blockCount; ++i)
      assert(BaseBlock::s_blocks[i].drop());

    delete[] BaseBlock::s_blocks;
    BaseBlock::s_blocks = nullptr;
  }

  BaseBlock::s_blockCount = 0;
  BaseBlock::s_blockLookupTable.clear();
  BaseBlock::s_packages.clear();
}

void BaseBlock::LoadBlockManifest() {
  BaseBlock::UnloadBlocks();
  bool manifestChanged = false;
  std::string path = World::GetWorldDir() + "block_manifest";
  std::ifstream manifest(path.c_str(), std::ios::binary);
  if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
    std::cout << "No Manifest Found, Creating Default Manifest" << std::endl;
    manifestChanged = true;
    BaseBlock::s_packages.insert(std::string("px"));
    BaseBlock::s_packages.insert(std::string("default"));
  } else {
    uint16_t version;
    manifest.read((char*)&version, sizeof(uint16_t));
    assert(version == BaseBlock::MANIFEST_VERSION);

    //Load Block Mapping
    manifest.read((char*)&BaseBlock::s_blockCount,
                  sizeof(BaseBlock::s_blockCount));
    std::cout << "Loading " << BaseBlock::s_blockCount
              << " Blocks from Manifest" << std::endl;
    BaseBlock::s_blocks = new BaseBlock[BaseBlock::s_blockCount + 1];
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
    
    //Load Packages
    uint16_t packageCount;
    manifest.read((char*)&packageCount, sizeof(uint16_t));
    std::cout << "Loading " << packageCount << " Packages from Manifest" << std::endl;
    assert(LoadPackage("default", false));
    for (uint16_t i = 0; i < packageCount; ++i) {
      uint8_t nameLength;
      manifest.read((char*)&nameLength, sizeof(uint8_t));
      char name[256];
      manifest.read(name, nameLength);
      name[nameLength] = '\0';
      std::string packageName = std::string(name);
      LoadPackage(packageName, false);
    }
  }

  manifest.close();
  if (manifestChanged) {
    UpdateManifest();
    std::cout << "Manifest Changed, Reloading Manifest" << std::endl;
    LoadBlockManifest();
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
  manifest.write((const char*)&BaseBlock::s_blockCount,
                 sizeof(BaseBlock::s_blockCount));
  // Write Block Loopup Table: id, name length, name
  for (std::pair<const std::string, uint32_t>& pair :
       BaseBlock::s_blockLookupTable) {
    manifest.write((const char*)&pair.second, sizeof(uint32_t));
    uint8_t length = pair.first.length();
    manifest.write((const char*)&length, 1);
    manifest.write(pair.first.c_str(), length);
  }

  // Write Package Count
  uint16_t packageCount = BaseBlock::s_packages.size();
  manifest.write((const char*)&packageCount, sizeof(packageCount));

  // Write Packages: name length, name
  for (const std::string& package : BaseBlock::s_packages) {
    uint8_t length = package.length();
    manifest.write((const char*)&length, 1);
    manifest.write(package.c_str(), length);
  }

  // Write null to end for check when reading
  const char nullChar = '\0';
  manifest.write(&nullChar, 1);
  manifest.close();
  std::cout << "Updated Block Manifest" << std::endl;
}

bool BaseBlock::LoadPackage(std::string name, bool updateManifest) {
  // Dont load package if its already loaded
  if (!BaseBlock::s_packages.empty() &&
      BaseBlock::s_packages.find(name) != BaseBlock::s_packages.end())
    return true;
  std::string path = World::GetAppAssetDir() + name + ".pxb";
  std::ifstream package(path.c_str(), std::ios::binary);
  if (!package || package.peek() == std::ifstream::traits_type::eof()) {
    package.close();
    path = World::GetAssetDir() + name + ".pxb";
    package.open(path.c_str(), std::ios::binary);

    if (!package || package.peek() == std::ifstream::traits_type::eof()) {
      package.close();
      std::cout << "Warrning, Failed to Load Block Package: " << name
                << std::endl;
      return false;
    }
  }

  uint16_t version;
  package.read((char*)&version, sizeof(uint16_t));
  if (version != BaseBlock::PACKAGE_VERSION) {
    package.close();
    std::cout << "Warrning, Failed to Load Block Package: " << name << ", Incorrect Package Version"
              << std::endl;
    return false;
  }

  BaseBlock::s_packages.insert(name);
  std::cout << "Loaded Block Package: " << name << " from " << path << std::endl;

  package.close();
  if (updateManifest) UpdateManifest();
  return true;
}
