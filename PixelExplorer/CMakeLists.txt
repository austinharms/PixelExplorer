## Copyright (c) 2024 Austin Harms
## License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

project("PixelExplorer" VERSION 0.0.0)
message(STATUS "PixelExplorer: Version ${PROJECT_VERSION}")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/plugin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/static)

add_executable (PixelExplorer "src/main.cpp"  "public/PE_mod_interface.h" "public/PE_defines.h" "public/PE_referenced.h" "public/PE_stdint.h" "src/PE_referenced.cpp" "public/PE_type_asserts.h"  "public/PE_memory.h" "src/PE_memory.cpp" "src/PE_mod_interface.cpp" "private/PE_mod_instance.h" "src/PE_mod_instance.cpp" "public/PE_errors.h" "private/PE_exception.h" "src/PE_exception.cpp" "private/PE_mod_loader.h" "src/PE_mod_loader.cpp" "public/PE_reference_utils.h" "public/PE_log.h" "src/PE_log.cpp")
set_target_properties(PixelExplorer PROPERTIES
  CXX_STANDARD 20
  ENABLE_EXPORTS true
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
target_include_directories(PixelExplorer PUBLIC "public")
target_include_directories(PixelExplorer PRIVATE "private")
target_compile_definitions(PixelExplorer PRIVATE PE_EXPORT_API=1)

# Preprocess to file
# set_property(TARGET PixelExplorer PROPERTY COMPILE_FLAGS "-P")

# Link dependencies
add_dependencies(PixelExplorer SDL3::SDL3)
target_link_libraries(PixelExplorer SDL3::SDL3)
add_dependencies(PixelExplorer glm::glm)
target_link_libraries(PixelExplorer glm::glm)
glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)
add_dependencies(PixelExplorer glad_gl_core_mx_33)
target_link_libraries(PixelExplorer glad_gl_core_mx_33)

# Create mods dir
add_custom_command(TARGET PixelExplorer POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${PROJECT_BINARY_DIR}/mods")

# Copy dependencies/resources to build dir
add_custom_command(TARGET PixelExplorer POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${CMAKE_CURRENT_SOURCE_DIR}/res" ${PROJECT_BINARY_DIR})
if (WIN32)
# TODO: this only works for windows
add_custom_command(TARGET PixelExplorer POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${SDL3_BINARY_DIR}/SDL3.dll" ${PROJECT_BINARY_DIR})
endif()
