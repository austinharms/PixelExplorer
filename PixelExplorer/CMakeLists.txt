project("PixelExplorer" VERSION 0.0.0)
message(STATUS "PixelExplorer: Version ${PROJECT_VERSION}")
add_executable (PixelExplorer "src/main.cpp")
set_property(TARGET PixelExplorer PROPERTY CXX_STANDARD 17)
target_include_directories(PixelExplorer PUBLIC "public")
target_include_directories(PixelExplorer PRIVATE "private")

# Link dependencies
add_dependencies(PixelExplorer SDL3::SDL3)
target_link_libraries(PixelExplorer SDL3::SDL3)
add_dependencies(PixelExplorer glm::glm)
target_link_libraries(PixelExplorer glm::glm)
glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)
add_dependencies(PixelExplorer glad_gl_core_mx_33)
target_link_libraries(PixelExplorer glad_gl_core_mx_33)

# Copy dependencies/resources to build dir
add_custom_command(TARGET PixelExplorer POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${CMAKE_CURRENT_SOURCE_DIR}/res" ${PROJECT_BINARY_DIR})
if (WIN32)
add_custom_command(TARGET PixelExplorer POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy "${SDL3_BINARY_DIR}/SDL3.dll" ${PROJECT_BINARY_DIR})
endif()
