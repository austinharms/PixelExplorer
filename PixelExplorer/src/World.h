#ifndef WORLD_H
#define WORLD_H
#include <string>
#include <thread>
#include <vector>

#include "package/Package.h"
#include "package/PackageLoader.h"
#include "chunk/ChunkManager.h"
#include "rendering/Renderer.h"

class World {
 public:
  static const uint16_t MANIFEST_VERSION = 1;

  // Should ONLY be called on the MAIN thread
  static void LoadWorld();
  // Should ONLY be called on the MAIN thread
  static void UnloadWorld();

  static void SetPhysicsPaused(bool paused) { s_physXPause = paused; }

  static bool GetPhysicsPaused() { return s_physXPause; }

  static const std::string GetWorldDir() { return s_worldDir; }

  static const std::vector<Package*> GetPackages() { return s_packages; }

  static const Package* GetPackage(std::string name) {
    std::vector<Package*>::iterator it;
    for (it = s_packages.begin(); it != s_packages.end(); ++it)
      if ((*it)->GetName() == name) return *it;
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

 private:
  World() {}
  ~World() {}
  static void updateManifest();
  static void physXUpdateLoop();
  static std::string s_worldDir;
  static volatile bool s_physXActive;
  static volatile bool s_physXPause;
  static std::vector<Package*> s_packages;
  static ChunkManager* s_chunkManager;
  static Renderer* s_renderer;
  static std::thread* s_physXThread;
  static std::atomic<float> s_physXAccumulator;
  static const float s_physXTimeStep;
};
#endif