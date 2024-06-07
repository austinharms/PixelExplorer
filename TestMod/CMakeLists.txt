project("TestMod" VERSION 0.0.0)
message(STATUS "Test Mod: Version ${PROJECT_VERSION}")

# Mods MUST be built as a shared lib
add_library(TestMod SHARED MODULE "src/mod.cpp" "public/TMOD_test_mod.h" "src/TMOD_test_mod_imp.cpp" "private/TMOD_test_mod_imp.h")
set_property(TARGET TestMod PROPERTY CXX_STANDARD 20)
target_include_directories(TestMod PUBLIC "public")
target_include_directories(TestMod PRIVATE "private")
target_link_libraries(TestMod PixelExplorer)

# Copy mod to PixelExplorer/mods
# TODO: this only works for windows
if (WIN32)
add_custom_command(TARGET TestMod POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${PROJECT_BINARY_DIR}/TestMod.dll" "${PixelExplorer_BINARY_DIR}/mods/")
endif()
