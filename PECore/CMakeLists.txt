project("PECore" VERSION 0.0.0)
message(STATUS "PECore: Version ${PROJECT_VERSION}")
add_library(PECore  "src/PE_application_entry.cpp" "src/PE_event_loop.cpp" "src/PE_log.cpp" "public/PE_log.h" "public/PE_defines.h" "private/PE_event_loop.h" "public/PE_graphics.h"   "private/PE_generated_graphics_api.h" "src/PE_graphics.cpp" "src/PE_graphics_opengl.cpp"  "private/PE_worker_thread.h" "public/PE_memory.h" "src/PE_memory.cpp" "public/PE_errors.h" "src/PE_worker_thread.cpp"   "public/PE_asset_manager.h" "src/PE_asset_manager.cpp" "private/PE_work_queue.h" "src/PE_work_queue.cpp"  "private/PE_barrier.h" "src/PE_barrier.cpp" "private/opengl_graphics/PE_opengl_shader.h" "private/opengl_graphics/PE_opengl_graphics.h"  "private/opengl_graphics/PE_opengl_window.h" "private/opengl_graphics/PE_opengl_common.h" "private/opengl_graphics/PE_gl_mesh.h" "src/PE_referenced.cpp" "public/PE_type_asserts.h" "public/PE_reference_utils.h" "private/PE_global.h" "private/PE_application_entry.h"  "private/PE_graphics_implementation.h" "src/opengl_graphics/PE_opengl_window.cpp" "src/opengl_graphics/PE_opengl_graphics.cpp" "src/opengl_graphics/PE_opengl_shader.cpp" "src/PE_graphics_implementation.cpp" "src/PE_global.cpp" "private/PE_exception.h" "src/PE_exception.cpp")
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
# Add PE_generated_graphics_api.h gerneration build step
add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/private/PE_generated_graphics_api.h" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/public/PE_graphics.h" COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_graphics_api.py ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
# TODO Make this a dynamic list
set(PECORE_DEPENDENCIES "${SDL3_BINARY_DIR}/SDL3.dll" "${CMAKE_CURRENT_SOURCE_DIR}/res")
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
