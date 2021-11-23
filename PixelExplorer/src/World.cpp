#include "World.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include "chunk/block/BaseBlock.h"

std::string World::s_appAssetDir;
std::string World::s_assetDir;
std::string World::s_worldDir;
std::set<std::string> World::s_packages;
ChunkManager* World::s_chunkManager = nullptr;
Renderer* World::s_renderer = nullptr;

void World::LoadWorld() {
	UnloadWorld();
	assert(World::s_renderer != nullptr);
	World::s_appAssetDir = std::string("C:\\Users\\austi\\source\\repos\\PixelExplorer\\PixelExplorer\\assets\\");
	World::s_assetDir = std::string("C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\assets\\");
	World::s_worldDir = std::string("C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\save-'test world'\\");
	World::s_chunkManager = new ChunkManager(World::s_renderer);
	World::s_packages.insert(std::string("default"));

	std::string manifestPath = World::s_worldDir + "world_manifest";
	std::ifstream manifest(manifestPath.c_str(), std::ios::binary);
	if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
		std::cout << "No World Manifest Found, Creating Default Manifest" << std::endl;
		manifest.close();
		UpdateManifest();
	}
	else
	{
		uint16_t version;
		manifest.read((char*)&version, sizeof(uint16_t));
		assert(version == World::MANIFEST_VERSION);
		uint16_t packageCount;
		manifest.read((char*)&packageCount, sizeof(uint16_t));
		std::cout << "Loading " << packageCount << " Packages from World Manifest" << std::endl;
		for (uint16_t i = 0; i < packageCount; ++i) {
			uint8_t nameLength;
			manifest.read((char*)&nameLength, sizeof(uint8_t));
			char name[256];
			manifest.read(name, nameLength);
			name[nameLength] = '\0';
			World::s_packages.insert(std::string(name));
		}

		char end;
		manifest.read(&end, sizeof(char));
		if (end != '\0')
			std::cout << "Warning may have failed loading World Manifest, No trailing NULL " << std::endl;
		manifest.close();
	}

	BaseBlock::LoadBlockManifest();
}

void World::UnloadWorld()
{
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
	for (const std::string& package : World::s_packages) {
		uint8_t length = package.length();
		manifest.write((const char*)&length, 1);
		manifest.write(package.c_str(), length);
	}

	// Write null to end for check when reading
	const char nullChar = '\0';
	manifest.write(&nullChar, 1);
	manifest.close();
}