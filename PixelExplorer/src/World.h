#ifndef WORLD_H
#define WORLD_H
#include <string>
#include <set>

class World {
public:
	static const uint16_t MANIFEST_VERSION = 1;
	static void LoadWorld();
	static void UnloadWorld();

	static const std::string GetAppAssetDir() { return s_appAssetDir; }

	static const std::string GetAssetDir() { return s_assetDir; }

	static const std::string GetWorldDir() { return s_worldDir; }

	static const std::set<std::string> GetPackages() { return s_packages; }

private:
	World() {}
	~World() {}
	static void UpdateManifest();

	static std::string s_appAssetDir;
	static std::string s_assetDir;
	static std::string s_worldDir;
	static std::set<std::string> s_packages;
};
#endif