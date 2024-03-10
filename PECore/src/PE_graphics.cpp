#include "PE_graphics.h"
#include "PE_graphics_implementation.h"
#include "PE_graphics_opengl.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "PE_log.h"
#include <new>

namespace pecore::graphics {
	static implementation::GraphicsCommands* command_table = nullptr;

#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret)	\
    rc fn params { ret command_table->fn args; }
#include "PE_generated_graphics_api.h"
#undef PE_GENERATED_GRAPHICS_API

	int Init() {
		// TODO Add check for event thread
		if (command_table) {
			return PE_ERROR_ALREADY_INITIALIZED;
		}

		// TODO Replace this with a dynamic selection from all available adapters
		command_table = implementation::AllocateOpenGlImplementation();
		if (!command_table) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate GraphicsCommands table"));
			return PE_ERROR_OUT_OF_MEMORY;
		}

		return command_table->InitSystem();
	}

	void Quit() {
		// TODO Add check for event thread
		if (command_table) {
			command_table->QuitSystem();
			command_table = nullptr;
		}
	}
}
