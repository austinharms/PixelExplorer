// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_PRIVATE_MOD_LOADER_H_
#define PE_PRIVATE_MOD_LOADER_H_
#include <unordered_map>
#include <mutex>
#include "PE_defines.h"
#include "PE_mod_instance.h"

namespace pe::internal {
	class ModLoader {
	public:
		typedef std::unordered_map<std::u8string, ModInstance> ModMap;

		static ModLoader& GetInstance();

		void LoadModsFolder();
		void UnloadAllMods();
		// TODO: how to make this call thread safe
		//const ModMap& GetLoadedMods();
		~ModLoader();
		PE_NOCOPY(ModLoader);

	private:
		std::unordered_map<std::u8string, ModInstance> mod_map_;
		std::mutex mod_map_mutex_;

		ModLoader();
	};
}
#endif // !PE_PRIVATE_MOD_LOADER_H_
