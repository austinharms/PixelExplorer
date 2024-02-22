project("PECore" VERSION 0.0.0)
message(STATUS "PECore: Version ${PROJECT_VERSION}")
add_library(PECore "public/PECore.h" "src/PECore.cpp" "src/PE_event_loop.cpp" "src/PE_log.cpp" "public/PE_log.h" "public/PE_defines.h" "private/PE_event_loop.h" "public/PE_graphics.h" "private/PE_opengl_adapter.h" "private/PE_graphics_adapter.h" "private/PE_graphics_api.h" "src/PE_graphics.cpp" "src/PE_opengl_adapter.cpp")
set_property(TARGET PECore PROPERTY CXX_STANDARD 17)
# Preprocess to file
# set_property(TARGET PECore PROPERTY COMPILE_FLAGS "-P")
target_include_directories(PECore PUBLIC "public")
target_include_directories(PECore PRIVATE "private")
add_dependencies(PECore SDL3::SDL3)
target_link_libraries(PECore SDL3::SDL3)
add_dependencies(PECore glm::glm)
target_link_libraries(PECore glm::glm)

if (WIN32)
# TODO Make this a dynamic list
set(PECORE_DEPENDENCIES "${SDL3_BINARY_DIR}/SDL3.dll")
endif()

set(PECORE_DEPENDENCIES ${PECORE_DEPENDENCIES} CACHE FORCE "PECore Dependencies")
message(STATUS "PECore: Dependencies ${PECORE_DEPENDENCIES}")
function(CopyPECoreDependencies caller dest)
    foreach( dep ${PECORE_DEPENDENCIES})
        add_custom_command(
            TARGET ${caller}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${dep} ${dest}
        )
    endforeach(dep)
endfunction()
