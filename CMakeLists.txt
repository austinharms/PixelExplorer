## Copyright (c) 2024 Austin Harms
## License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

cmake_minimum_required (VERSION 3.14)
project("PixelExplorerCore")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Link SDL and PhysX dynamicly
set(BUILD_SHARED_LIBS true)
set(SDL_SHARED true)
set(SDL_STATIC false)
add_subdirectory("Vendor/SDL")

# Link glm and glad statically
set(BUILD_SHARED_LIBS false)
add_subdirectory("Vendor/glad/cmake")
add_subdirectory("Vendor/glm")
add_subdirectory("PixelExplorer")
add_subdirectory("TestMod")
