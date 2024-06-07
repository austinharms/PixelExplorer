// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include <iostream>
#include "PE_log.h"
#include "PE_mod_loader.h"

using namespace pe;
using namespace pe::internal;
int main(int argc, char** argv) {
	PE_LogDebug(PE_TEXT("PixelExplorer start"));
	ModLoader& mod_loader = ModLoader::GetInstance();
	mod_loader.LoadModsFolder();

	mod_loader.UnloadAllMods();
	PE_LogDebug(PE_TEXT("PixelExplorer exit"));
	return 0;
}