cmake_minimum_required (VERSION 3.16)
project("PixelExplorerCore")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(BUILD_SHARED_LIBS false)
set(SDL_SHARED true)
set(SDL_STATIC false)

add_subdirectory("Vendor/SDL")
add_subdirectory("Vendor/glad/cmake")
add_subdirectory("Vendor/glm")
add_subdirectory("PixelExplorer")