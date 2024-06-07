// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include "PE_mod_loader.h"
#include <filesystem>
#include "PE_log.h"
#include "PE_exception.h"

namespace pe::internal {
	ModLoader& ModLoader::GetInstance()
	{
		static ModLoader mod_loader;
		return mod_loader;
	}

	ModLoader::ModLoader() {

	}

	ModLoader::~ModLoader()
	{
	}

	void ModLoader::LoadModsFolder()
	{
		std::lock_guard lock(mod_map_mutex_);
		std::filesystem::path mod_dir = std::filesystem::current_path() / "mods";
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(mod_dir)) {
			if (entry.is_regular_file()) {
				try {
					ModInstance mod(entry.path());
					const ModInterface::Details& mod_details = mod.GetModInterface().GetDetails();
					mod_map_.emplace(mod_details.name, std::move(mod));
					PE_LogInfo(PE_TEXT("Loaded Mod: %s"), mod_details.name);
				}
				catch (...) {
					KnownExceptionHandler();
				}
			}
		}
	}

	void ModLoader::UnloadAllMods()
	{
	}
}
