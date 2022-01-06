#ifndef WORLD_H
#define WORLD_H
#include <string>
#include <thread>
#include <vector>

#include "Package.h"
#include "chunk/ChunkManager.h"
#include "rendering/Renderer.h"

class World {
 public:
  static const uint16_t MANIFEST_VERSION = 1;
  static void LoadWorld();
  static void UnloadWorld();

  static void SetPhysicsPaused(bool paused) { s_physXPause = paused; }

  static bool GetPhysicsPaused() { return s_physXPause; }

  static const std::string GetAppAssetDir() { return s_appAssetDir; }

  static const std::string GetAssetDir() { return s_assetDir; }

  static const std::string GetWorldDir() { return s_worldDir; }

  static const std::vector<Package> GetPackages() { return s_packages; }

  static const Package* GetPackage(std::string name) {
    std::vector<Package>::iterator it;
    for (it = s_packages.begin(); it != s_packages.end(); ++it)
      if ((*it).getName() == name) return &*it;
    return nullptr;
  }

  static ChunkManager* GetChunkManager() { return s_chunkManager; }

  static void SetRenderer(Renderer* renderer) {
    if (s_renderer != nullptr) s_renderer->drop();
    s_renderer = renderer;
    if (s_renderer != nullptr) s_renderer->grab();
  }

  static void AddSimulationTime(float time) {
    if (!s_physXPause && s_physXActive)
      s_physXAccumulator = s_physXAccumulator + time;
  }

  // static void StepPhysics(float time = 0.05f) {
  //  s_chunkManager->getScene()->simulate(time);
  //  s_chunkManager->getScene()->fetchResults(true);
  //}

 private:
  World() {}
  ~World() {}
  static void updateManifest();
  static bool dirExists(const char* path);
  static void physXUpdateLoop();
  static volatile bool s_physXActive;
  static volatile bool s_physXPause;
  static std::string s_appAssetDir;
  static std::string s_assetDir;
  static std::string s_worldDir;
  static std::vector<Package> s_packages;
  static ChunkManager* s_chunkManager;
  static Renderer* s_renderer;
  static std::thread* s_physXThread;
  static std::atomic<float> s_physXAccumulator;
  static const float s_physXTimeStep;
};
#endif