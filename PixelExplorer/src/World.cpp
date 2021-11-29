#include "World.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include "chunk/block/BaseBlock.h"

std::string World::s_appAssetDir;
std::string World::s_assetDir;
std::string World::s_worldDir;
std::set<Package> World::s_packages;
ChunkManager* World::s_chunkManager = nullptr;
Renderer* World::s_renderer = nullptr;

void World::LoadWorld() {
  UnloadWorld();
  assert(World::s_renderer != nullptr);
  World::s_appAssetDir = std::string(
      "C:"
      "\\Users\\austi\\source\\repos\\PixelExplorer\\PixelExplorer\\assets\\");
  World::s_assetDir = std::string(
      "C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\assets\\");
  World::s_worldDir = std::string(
      "C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\save-'test "
      "world'\\");
  World::s_chunkManager = new ChunkManager(World::s_renderer);
  World::s_packages.insert(
      Package(std::string("default"), World::MANIFEST_VERSION));

  std::string manifestPath = World::s_worldDir + "world_manifest";
  std::ifstream manifest(manifestPath.c_str(), std::ios::binary);
  if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
    std::cout << "No World Manifest Found, Creating Default Manifest"
              << std::endl;
    manifest.close();
    UpdateManifest();
  } else {
    uint16_t version;
    manifest.read((char*)&version, sizeof(uint16_t));
    assert(version == World::MANIFEST_VERSION);
    uint16_t packageCount;
    manifest.read((char*)&packageCount, sizeof(uint16_t));
    std::cout << "Loading " << packageCount << " Packages from World Manifest"
              << std::endl;
    for (uint16_t i = 0; i < packageCount; ++i) {
      uint8_t nameLength;
      manifest.read((char*)&nameLength, sizeof(uint8_t));
      char name[256];
      manifest.read(name, nameLength);
      name[nameLength] = '\0';
      World::s_packages.insert(Package(std::string(name), version));
    }

    char end;
    manifest.read(&end, sizeof(char));
    if (end != '\0')
      std::cout
          << "Warning may have failed loading World Manifest, No trailing NULL "
          << std::endl;
    manifest.close();
  }

  for (const Package& package : World::s_packages) {
    std::string path = World::s_appAssetDir + package.getName();
    if (dirExists(path.c_str())) {
      package.setPath(path + "\\");
    }
    else {
      path = World::s_assetDir + package.getName();
      if (dirExists(path.c_str())) {
        package.setPath(path + "\\");
      } else {
        std::cout << "Failed to Find Package " << package.getName()
                  << std::endl;
        continue;
      }
    }

    std::cout << "Found Package " << package.getName() << " at " << path << std::endl;
  }

  BaseBlock::LoadBlockManifest();
  Shader* chunkShader = new Shader("./res/shaders/Chunk.shader");
  assert(chunkShader->isValid());
  Chunk::SetChunkMaterialShader(chunkShader);
  chunkShader->drop();
  chunkShader = nullptr;
}

void World::UnloadWorld() {
  BaseBlock::UnloadBlocks();
  World::s_packages.clear();
  World::s_appAssetDir.clear();
  World::s_assetDir.clear();
  World::s_worldDir.clear();
  if (World::s_chunkManager != nullptr) {
    World::s_chunkManager->UnloadChunks();
    World::s_chunkManager->drop();
    World::s_chunkManager = nullptr;
  }
}

void World::UpdateManifest() {
  std::string manifestPath = World::s_worldDir + "world_manifest";
  std::ofstream manifest(manifestPath.c_str(), std::ios::binary);
  manifest.write((const char*)&World::MANIFEST_VERSION, sizeof(uint16_t));
  uint16_t packageCount = World::s_packages.size();
  manifest.write((const char*)&packageCount, sizeof(packageCount));
  for (const Package& package : World::s_packages) {
    uint8_t length = package.getName().length();
    manifest.write((const char*)&length, 1);
    manifest.write(package.getName().c_str(), length);
  }

  // Write null to end for check when reading
  const char nullChar = '\0';
  manifest.write(&nullChar, 1);
  manifest.close();
}

bool World::dirExists(const char* path) {
  struct stat info;
  return stat(path, &info) == 0 && (info.st_mode & S_IFDIR);
}
