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
            uint8_t solid;
            blockFile.read((char*)&solid, sizeof(uint8_t));
            b._solid = solid != 0;
            for (uint8_t j = 0; j < (uint8_t)FaceDirection::FACECOUNT; ++j) {
              uint8_t faceDir;
              blockFile.read((char*)&faceDir, sizeof(uint8_t));
              uint8_t full;
              blockFile.read((char*)&full, sizeof(uint8_t));
              b._faces[faceDir].full = full != 0;
              uint8_t transparent;
              blockFile.read((char*)&transparent, sizeof(uint8_t));
              b._faces[faceDir].transparent = transparent != 0;
              uint8_t vertexCount;
              blockFile.read((char*)&vertexCount, sizeof(uint8_t));
              b._faces[faceDir].vertexCount = vertexCount;
              b._faces[faceDir].vertices = new float[vertexCount * 3];
              blockFile.read((char*)(b._faces[faceDir].vertices),
                             sizeof(float) * vertexCount * 3);
              b._faces[faceDir].uvs = new float[vertexCount * 2];
              blockFile.read((char*)(b._faces[faceDir].uvs),
                             sizeof(float) * vertexCount * 2);
              uint8_t indexCount;
              blockFile.read((char*)&indexCount, sizeof(uint8_t));
              b._faces[faceDir].indexCount = indexCount;
              b._faces[faceDir].indices = new uint8_t[indexCount];
              blockFile.read((char*)(b._faces[faceDir].indices),
                             sizeof(uint8_t) * indexCount);
            }

            b._name = blockNames[i];
            BaseBlock::LoadBlock(b);
          }

          delete[] blockNames;
          char end;
          blockFile.read(&end, sizeof(char));
          if (end != 0) {
            Logger::Warn("May have failed loading block module from: \"" +
                         packagePath +
                         "/blocks.module\", File did not end with NULL");
          } else {
            Logger::Info("Loaded block module: \"" + packagePath +
                         "/blocks.module\", Loaded " +
                         std::to_string(blockCount) + " blocks");
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
