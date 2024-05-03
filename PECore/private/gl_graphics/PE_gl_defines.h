#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_DEFINES_H_
#define PE_GL_GRAPHICS_GL_DEFINES_H_
#include "PE_defines.h"
// Should windows use there own OpenGl context and render thread
// If false all windows will render using a single context on the main/event thread
#ifndef PE_GL_CONTEXT_PER_WINDOW
#define PE_GL_CONTEXT_PER_WINDOW 0
#endif // !PE_GL_CONTEXT_PER_WINDOW

// Enables background loading using a worker thread and context
#ifndef PE_GL_BACKGROUND_LOADING
#define PE_GL_BACKGROUND_LOADING 0
#endif // !PE_GL_BACKGROUND_LOADING
#endif // !PE_GL_GRAPHICS_GL_DEFINES_H_
#endif // !PE_DISABLE_OPENGL
