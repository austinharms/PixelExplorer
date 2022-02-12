#include "World.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include "FileUtilities.h"
#include "Logger.h"
#include "chunk/block/BaseBlock.h"

std::string World::s_worldDir = "";
std::vector<Package*> World::s_packages;
volatile bool World::s_physXActive = false;
volatile bool World::s_physXPause = false;
ChunkManager* World::s_chunkManager = nullptr;
Renderer* World::s_renderer = nullptr;
std::thread* World::s_physXThread = nullptr;
std::atomic<float> World::s_physXAccumulator = 0;
const float World::s_physXTimeStep = 0.01;

void World::LoadWorld() {
  UnloadWorld();
  s_worldDir = FileUtilities::GetResourceDirectory() + "/worlds/test-world/";
#ifdef DEBUG
  assert(World::s_renderer != nullptr);
#else
  if (World::s_renderer == nullptr) {
    Logger::Error("World Renderer Not Set");
    abort();
  }
#endif  // DEBUG

  World::s_chunkManager = new ChunkManager(World::s_renderer);
  BaseBlock::StartBlockLoading();
  Package* defaultPackage = PackageLoader::LoadPackageByName("default");
#ifdef DEBUG
  assert(defaultPackage != nullptr && defaultPackage->GetLoaded());
#else
  if (defaultPackage == nullptr || !defaultPackage->GetLoaded()) {
    Logger::Error("Failed to Load Required \"default\" Package");
    abort();
  }
#endif  // DEBUG

  World::s_packages.emplace_back(defaultPackage);
  std::string manifestPath = World::s_worldDir + "world.manifest";
  std::ifstream manifest(manifestPath.c_str(), std::ios::binary);
  if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
    manifest.close();

    Logger::Warn("No World Manifest Found, Creating Default Manifest");
    Package* pxPackage = PackageLoader::LoadPackageByName("px");
    if (pxPackage != nullptr) {
      World::s_packages.push_back(pxPackage);
    } else {
      Logger::Warn("Failed to Load px Package When Creating Default Manifest");
    }

    updateManifest();
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
      Package* pkg = PackageLoader::LoadPackageByName(std::string(name));
      if (pkg != nullptr) World::s_packages.push_back(pkg);
    }

    char end;
    manifest.read(&end, sizeof(char));
    if (end != '\0')
      Logger::Warn("May Have Failed Loading World Manifest, No trailing NULL");
    manifest.close();
  }

  BaseBlock::FinalizeBlockLoading();
  Shader* chunkShader = new Shader("./res/shaders/Chunk.shader");
#ifdef DEBUG
  assert(chunkShader->isValid());
#else
  if (!chunkShader->isValid()) {
    Logger::Error("Failed to Load Chunk Shader");
    abort();
  }
#endif  // DEBUG
  Chunk::SetChunkMaterialShader(chunkShader);
  chunkShader->drop();
  chunkShader = nullptr;

  if (World::s_physXThread != nullptr)
    Logger::Warn("Old PhysX Thread Not Cleaned Up");

  World::s_physXPause = false;
  World::s_physXActive = true;
  World::s_physXThread = new std::thread(&World::physXUpdateLoop);
}

void World::UnloadWorld() {
  World::s_physXPause = true;
  World::s_physXActive = false;
  if (World::s_physXThread != nullptr) {
    World::s_physXThread->join();
    delete World::s_physXThread;
    World::s_physXThread = nullptr;
  }

  if (World::s_chunkManager != nullptr) {
    // Call here to ensure it is done on the main thread
    World::s_chunkManager->unloadChunks();
    World::s_chunkManager->drop();
    World::s_chunkManager = nullptr;
  }

  BaseBlock::UnloadBlocks();
  for (Package* package : World::s_packages) package->drop();

  World::s_packages.clear();
  World::s_worldDir.clear();
}

void World::updateManifest() {
  std::string manifestPath = World::s_worldDir + "world_manifest";
  std::ofstream manifest(manifestPath.c_str(), std::ios::binary);
  manifest.write((const char*)&World::MANIFEST_VERSION, sizeof(uint16_t));
  uint16_t packageCount = World::s_packages.size();
  manifest.write((const char*)&packageCount, sizeof(packageCount));
  for (const Package* package : World::s_packages) {
    uint8_t length = package->GetName().length();
    manifest.write((const char*)&length, 1);
    manifest.write(package->GetName().c_str(), length);
  }

  // Write null to end for check when reading
  const char nullChar = '\0';
  manifest.write(&nullChar, 1);
  manifest.close();
}

void World::physXUpdateLoop() {
  while (World::s_physXActive) {
    if (!World::s_physXPause) {
      if (World::s_physXAccumulator >= World::s_physXTimeStep) {
        World::s_physXAccumulator =
            World::s_physXAccumulator - World::s_physXTimeStep;
        s_chunkManager->getScene()->simulate(World::s_physXTimeStep);
        s_chunkManager->getScene()->fetchResults(true);
      }
    }
  }
}
