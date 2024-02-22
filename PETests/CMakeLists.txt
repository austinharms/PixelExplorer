project("PETests" VERSION 0.0.0)
message(STATUS "PETests: Version ${PROJECT_VERSION}")
# Hack to include private PECore headers to allow testing
get_target_property(PECORE_INCLUDE_DIRECTORIES PECore INCLUDE_DIRECTORIES)

add_executable(PETests "src/test.cpp")
set_property(TARGET PETests PROPERTY CXX_STANDARD 17)
target_include_directories(PETests PRIVATE "private" ${PECORE_INCLUDE_DIRECTORIES})
add_dependencies(PETests PECore)
target_link_libraries(PETests PRIVATE PECore)
add_test(NAME PETests COMMAND PETests)
CopyPECoreDependencies(PETests ${PROJECT_BINARY_DIR})
