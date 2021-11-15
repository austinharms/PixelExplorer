#include "BaseBlock.h"
#include <iostream>

BaseBlock* BaseBlock::s_blocks = nullptr;
uint32_t BaseBlock::s_blockCount = 0;
std::vector<std::string> BaseBlock::s_packages = std::vector<std::string>();
std::unordered_map<std::string, uint32_t> BaseBlock::s_blockLookupTable =
    std::unordered_map<std::string, uint32_t>();

BaseBlock::BaseBlock() {}

BaseBlock::~BaseBlock() {}

void BaseBlock::LoadBlockManifest() {
  bool manifestChanged = false;
  std::string path = World::GetWorldDir() + "block_manifest";
  std::ifstream manifest(path.c_str(), std::ios::binary);
  if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
    manifestChanged = true;
    assert(LoadPackage("default", false));
    LoadPackage("px", false);
  } else {
    uint16_t version;
    manifest.read((char*)&version, sizeof(uint16_t));
    assert(version == BaseBlock::VERSION);

    manifest.read((char*)&BaseBlock::s_blockCount,
                  sizeof(BaseBlock::s_blockCount));
    std::cout << "Loading " << BaseBlock::s_blockCount << "Blocks from Manifest...";
    BaseBlock::s_blocks = new BaseBlock[BaseBlock::s_blockCount];
    for (uint32_t i = 0; i < BaseBlock::s_blockCount; ++i) {
      uint32_t blockId;
      manifest.read((char*)&blockId, sizeof(uint32_t));
      uint8_t nameLength;
      manifest.read((char*)&nameLength, sizeof(uint8_t));
      char name[256];
      manifest.read(name, nameLength);
      name[nameLength] = '\0';
      BaseBlock::s_blockLookupTable.insert({std::string(name), blockId});
    }

    std::cout << "Done" << std::endl;

    uint16_t packageCount;
    manifest.read((char*)&packageCount, sizeof(uint16_t));


  }

  manifest.close();
  if (manifestChanged) UpdateManifest();
}

void BaseBlock::UpdateManifest() {
  std::string path = World::GetWorldDir() + "block_manifest";
  // Version, Block Count,[block id, block name length, block name], Package
  // Count,[package name length, package name]
  std::ofstream manifest(path.c_str(), std::ios::binary | std::ios::trunc);

  //Write Manifest Version
  manifest.write((const char*)&BaseBlock::VERSION, sizeof(BaseBlock::VERSION));

  //Write Block Count
  manifest.write((const char*)&BaseBlock::s_blockCount,
                 sizeof(BaseBlock::s_blockCount));
  //Write Block Loopup Table: id, name length, name
  for (std::pair<const std::string, uint32_t>& pair : BaseBlock::s_blockLookupTable) {
    manifest.write((const char*)&pair.second, sizeof(uint32_t));
    uint8_t length = pair.first.length();
    manifest.write((const char*)&length, 1);
    manifest.write(pair.first.c_str(), length);
  }

  //Write Package Count
  uint16_t packageCount = BaseBlock::s_packages.size();
  manifest.write((const char*)&packageCount, sizeof(packageCount));

  //Write Packages: name length, name
  for (const std::string& package : BaseBlock::s_packages) {
    uint8_t length = package.length();
    manifest.write((const char*)&length, 1);
    manifest.write(package.c_str(), length);
  }

  //Write null to end for check when reading
  const char nullChar = '\0';
  manifest.write(&nullChar, 1);
  manifest.close();
  std::cout << "Updated Block Manifest" << std::endl;
}

bool BaseBlock::LoadPackage(std::string name, bool updateManifest) {
  std::string path = World::GetAppAssetDir() + name + ".pxb";
  std::ifstream package(path.c_str(), std::ios::binary);
  if (!package || package.peek() == std::ifstream::traits_type::eof()) {
    package.close();
    path = World::GetAssetDir() + name + ".pxb";
    package.open(path.c_str(), std::ios::binary);

    if (!package || package.peek() == std::ifstream::traits_type::eof()) {
      package.close();
      std::cout << "Warrning, Failed to Load Block Package: " << name << std::endl;
      return false;
    }
  }

  BaseBlock::s_packages.push_back(name);
  std::cout << "Loaded Block Package: " << name << std::endl;

  package.close();
  if (updateManifest) UpdateManifest();
  return true;
}
