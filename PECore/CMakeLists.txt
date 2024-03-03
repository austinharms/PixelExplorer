project("PECore" VERSION 0.0.0)
message(STATUS "PECore: Version ${PROJECT_VERSION}")
add_library(PECore "public/PECore.h" "src/PECore.cpp" "src/PE_event_loop.cpp" "src/PE_log.cpp" "public/PE_log.h" "public/PE_defines.h" "private/PE_event_loop.h" "public/PE_graphics.h" "private/PE_opengl_adapter.h" "private/PE_graphics_adapter.h" "private/PE_dgapi.h" "src/PE_graphics.cpp" "src/PE_opengl_adapter.cpp" "public/PE_graphics_types.h" "private/PE_worker_thread.h" "private/PE_memory.h" "src/PE_memory.cpp" "public/PE_errors.h" "src/PE_worker_thread.cpp" "private/PE_asset_manager_internal.h" "src/PE_asset_manager_internal.cpp" "public/PE_asset_manager.h" "src/PE_asset_manager.cpp" "public/PE_work_queue.h" "src/PE_work_queue.cpp")
set_property(TARGET PECore PROPERTY CXX_STANDARD 17)
# Preprocess to file
# set_property(TARGET PECore PROPERTY COMPILE_FLAGS "-P")
target_include_directories(PECore PUBLIC "public")
target_include_directories(PECore PRIVATE "private")
add_dependencies(PECore SDL3::SDL3)
target_link_libraries(PECore SDL3::SDL3)
add_dependencies(PECore glm::glm)
target_link_libraries(PECore glm::glm)
glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)
add_dependencies(PECore glad_gl_core_mx_33)
target_link_libraries(PECore glad_gl_core_mx_33)
# Add PE_dgapi.h gerneration build step
add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/private/PE_dgapi.h" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/public/PE_graphics.h" COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/tools/genDGAPI.py ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
# TODO Make this a dynamic list
set(PECORE_DEPENDENCIES "${SDL3_BINARY_DIR}/SDL3.dll")
endif()

set(PECORE_DEPENDENCIES ${PECORE_DEPENDENCIES} CACHE INTERNAL "PECore Dependencies")
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
