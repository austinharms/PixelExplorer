// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include "PE_mod_instance.h"
// TODO: Make this file mac/linux compatible
#include <windows.h>
#include <new>
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_exception.h"

namespace pe::internal {
	void* ModInstance::LoadLib(const std::filesystem::path& path) PE_NOEXCEPT
	{
		// TODO: Make this file mac/linux compatible
		return LoadLibraryW(path.c_str());
	}

	void ModInstance::UnloadLib(void* lib) PE_NOEXCEPT
	{
		// TODO: Make this file mac/linux compatible
		FreeLibrary(static_cast<HMODULE>(lib));
	}

	ModInstance::ModInstance(const std::filesystem::path& path) :
		library_handle_(nullptr),
		mod_interface_(nullptr) {
		library_handle_ = LoadLib(path);
		if (!library_handle_) {
			throw ErrorCodeException(PE_ERROR_NOT_FOUND);
		}

		ModInterface::ModInterfaceFactoryFunctionPtr factory_function = (ModInterface::ModInterfaceFactoryFunctionPtr)GetProcAddress(static_cast<HMODULE>(library_handle_), PE_GENERIC_TEXT(PE_MOD_INTERFACE_FACTORY_FUNCTION_NAME));
		if (!factory_function) {
			UnloadLib(library_handle_);
			library_handle_ = nullptr;
			throw ErrorCodeException(PE_ERROR_INVALID_PARAMETERS);
		}

		mod_interface_ = factory_function();
		if (mod_interface_.GetValue()) {
			// Call drop here as mod_interface_ has grabed the value for us and we don't want extra references
			mod_interface_.GetValue()->Drop();
		}
		else
		{
			factory_function = nullptr;
			UnloadLib(library_handle_);
			library_handle_ = nullptr;
			throw ErrorCodeException(PE_ERROR_GENERAL);
		}
	}

	ModInstance::ModInstance(ModInstance&& mod_instance) PE_NOEXCEPT :
		library_handle_(mod_instance.library_handle_),
		mod_interface_(std::move(mod_instance.mod_interface_)) {
		mod_instance.library_handle_ = nullptr;
	}

	ModInstance::~ModInstance()
	{
		// TODO: this assumes that when the ModInstance is dropped the ModInterface only has one ref
		// This is may not be true and WILL cause errors as the lib will be unloaded while still in use
		mod_interface_ = nullptr;
		if (library_handle_) {
			UnloadLib(library_handle_);
			library_handle_ = nullptr;
		}
	}

	PE_NODISCARD ModInterface& ModInstance::GetModInterface()
	{
		return *mod_interface_;
	}

	PE_NODISCARD const ModInterface& ModInstance::GetModInterface() const
	{
		return *mod_interface_;
	}
}