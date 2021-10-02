#include "World.h"

const char* World::_appAssetDir;
const char* World::_assetDir;
const char* World::_worldDir;

void World::LoadWorld() { 
  _appAssetDir =
      "C:\\Users\\austi\\source\\repos\\PixelExplorer\\PixelExplorer\\res";
  _assetDir =
      "C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\assets";
  _worldDir =
      "C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\save-'test world'";
}