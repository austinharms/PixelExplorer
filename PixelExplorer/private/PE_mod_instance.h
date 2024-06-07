// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_PRIVATE_MOD_INSTANCE_H_
#define PE_PRIVATE_MOD_INSTANCE_H_
#include <filesystem>
#include "PE_defines.h"
#include "PE_mod_interface.h"
#include "PE_reference_utils.h"

namespace pe::internal {
	class ModLoader;
	class ModInstance {
	public:
		PE_NODISCARD ModInterface& GetModInterface();
		PE_NODISCARD const ModInterface& GetModInterface() const;
		ModInstance(ModInstance&& mod_instance) PE_NOEXCEPT;
		virtual ~ModInstance();
		PE_NOCOPY(ModInstance);

	private:
		friend class ModLoader;
		void* library_handle_;
		ReferencePtr<ModInterface> mod_interface_;

		// Loads the library into the process and returns a handle to it or nullptr on failure
		static PE_NODISCARD void* LoadLib(const std::filesystem::path& path) PE_NOEXCEPT;

		// Unloads libraries loaded with LoadLib
		static void UnloadLib(void* lib) PE_NOEXCEPT;

		ModInstance(const std::filesystem::path& path);
	};
}
#endif // !PE_PRIVATE_MOD_INSTANCE_H_
