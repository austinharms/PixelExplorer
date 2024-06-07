// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_MOD_INTERFACE_H_
#define PE_MOD_INTERFACE_H_
#include "PE_defines.h"
#include "PE_stdint.h"
#include "PE_referenced.h"

#define PE_MOD_INTERFACE_FACTORY_FUNCTION_NAME PE_MOD_ModInterfaceFactory
// This macro should be used as the name of the function that returns the mods ModInterface object
// ex: PE_ModInterfaceFactoryFunction() { return new ModImplementation(); }
// Note the lack of return type as the macro handles that for you
#define PE_ModInterfaceFactoryFunction PE_EXTERN_C PE_EXPORT_FROM_LIB pe::ModInterface* PE_CALL PE_MOD_INTERFACE_FACTORY_FUNCTION_NAME
namespace pe {
	//PE_EXTERN_C PE_API ModInterface* PE_CALL GetModInterface();
	class PE_API ModInterface : public Referenced
	{
	public:
		typedef ModInterface* (*ModInterfaceFactoryFunctionPtr)();
		struct Details
		{
			const uint64_t version;
			const char8_t* name;
			const char8_t* description;
			// TODO: check if mod is compatible with current version
			//uint64_t max_game_version;
			//uint64_t min_game_version;
		};

		// Returns the mods details
		// Note: this will be called BEFORE Init and should be setup the in constructor
		virtual const Details& GetDetails() = 0;

		// Setup and enables the mod
		// Note: any mod setup should be run at this time NOT in the constructor
		virtual void Init() = 0;

		// Note: any mod setup should NOT be in the constructor
		ModInterface();
		virtual ~ModInterface();
		PE_NOCOPY(ModInterface);
	};
}
#endif // !PE_MOD_INTERFACE_H_
