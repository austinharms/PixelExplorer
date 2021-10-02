#ifndef WORLD_H
#define WORLD_H

class World {
 public:
  static void LoadWorld();

  static const char* AppAssetDir() { return _appAssetDir; }

  static const char* AssetDir() { return _assetDir; }

  static const char* WorldDir() { return _worldDir; }

 private:
  World() {}
  ~World() {}

  static const char* _appAssetDir;
  static const char* _assetDir;
  static const char* _worldDir;
};
#endif