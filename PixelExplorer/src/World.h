#ifndef WORLD_H
#define WORLD_H
#include <string>

class World {
 public:
  static void LoadWorld();

  static const std::string GetAppAssetDir() { return _appAssetDir; }

  static const std::string GetAssetDir() { return _assetDir; }

  static const std::string GetWorldDir() { return _worldDir; }

 private:
  World() {}
  ~World() {}

  static std::string _appAssetDir;
  static std::string _assetDir;
  static std::string _worldDir;
};
#endif