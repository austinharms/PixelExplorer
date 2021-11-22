#include "World.h"
#include <iostream>
#include <cassert>
#include <fstream>

std::string World::s_appAssetDir;
std::string World::s_assetDir;
std::string World::s_worldDir;
std::set<std::string> World::s_packages;

void World::LoadWorld() {
	World::s_appAssetDir = std::string("C:\\Users\\austi\\source\\repos\\PixelExplorer\\PixelExplorer\\assets\\");
	World::s_assetDir = std::string("C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\assets\\");
	World::s_worldDir = std::string("C:\\Users\\austi\\AppData\\Local\\PixelExplorer\\0.0.0\\save-'test world'\\");
	World::s_packages = std::set<std::string>();
	World::s_packages.insert(std::string("default"));

	std::string manifestPath = World::s_worldDir + "world_manifest";
	std::ifstream manifest(manifestPath.c_str(), std::ios::binary);
	if (!manifest || manifest.peek() == std::ifstream::traits_type::eof()) {
		std::cout << "No Manifest Found, Creating Default Manifest" << std::endl;
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
		std::cout << "Loading " << packageCount << " Packages from Manifest" << std::endl;
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
			std::cout << "Warning may have failed loading world manifest, No trailing NULL " << std::endl;
		manifest.close();
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