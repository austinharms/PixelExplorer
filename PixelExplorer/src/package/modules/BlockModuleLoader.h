#ifndef BLOCKMODULELOADER_H
#define BLOCKMODULELOADER_H
#include <iostream>
#include <string>

#include "FileUtilities.h"
#include "Logger.h"
#include "PackageModule.h"
#include "PackageModuleId.h"
#include "PackageModuleLoader.h"
#include "package/PackageLoader.h"

class BlockLoader : public PackageModuleLoader {
 public:
  PackageModule* Load(std::string packagePath) {
    if (FileUtilities::FileExists(packagePath + "/blocks.module")) {
      std::ifstream blockFile((packagePath + "/blocks.module").c_str(),
                              std::ios::binary);
      if (blockFile && blockFile.peek() != std::ifstream::traits_type::eof()) {
        uint16_t fileVersion;
        blockFile.read((char*)&fileVersion, sizeof(uint16_t));
        if (fileVersion == VERSION) {
          uint16_t blockCount;
          blockFile.read((char*)&blockCount, sizeof(uint16_t));
          Logger::Debug("Found " + std::to_string(blockCount) +
                        " blocks in Block Module");
          std::string* blockNames = new std::string[blockCount];
          for (uint16_t i = 0; i < blockCount; ++i) {
            blockNames[i] = "";
            char c = 0;
            do {
              c = blockFile.get();
              blockNames[i] += c;
            } while (c != NULL && c != std::ifstream::traits_type::eof());
            Logger::Debug("Found block: \"" + blockNames[i] +
                          "\" in block module");
          }

          BaseBlock::RegisterBlocks(blockNames, blockCount);
          for (uint16_t i = 0; i < blockCount; ++i) {
            BaseBlock b;
            b._name = blockNames[i];
            b._
          }

          blockFile.close();
          return PackageModuleLoader::CreateDummyModule(this->ModuleId());
        } else {
          Logger::Warn(
              "Failed to load block file from: \"" + packagePath +
              "/blocks.module\" due to incorrect file version, found: " +
              std::to_string(fileVersion) +
              ", expected: " + std::to_string(VERSION));
        }
      }

      blockFile.close();
    }

    return nullptr;
  }

  static void Register() {
    BlockLoader* loader = new BlockLoader();
    PackageLoader::RegisterModuleLoader(loader);
    loader->drop();
  }

  virtual ~BlockLoader() {}

 private:
  const uint16_t VERSION = 1;
  BlockLoader() : PackageModuleLoader(PackageModuleId::Id<BlockLoader>(), 4) {}
};
#endif  // !BLOCKMODULELOADER_H
