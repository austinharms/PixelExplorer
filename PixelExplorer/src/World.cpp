#include "World.h"

std::string World::_appAssetDir;
std::string World::_assetDir;
std::string World::_worldDir;

void World::LoadWorld() { 
  _appAssetDir = std::string("C:\\Users\\austi\\source\\repos\\PixelExplorer\\PixelExplorer\\assets\\");
  _assetDir = std::string("C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\assets\\");
  _worldDir = std::string("C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\save-'test world'\\");
}