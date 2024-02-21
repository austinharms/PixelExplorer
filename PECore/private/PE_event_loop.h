#ifndef PE_SDL_WRAPPER_H_
#define PE_SDL_WRAPPER_H_
#include "PE_defines.h"

PE_EXTERN_C PE_API void PE_CALL PE_PrepareSDLEventLoop();
PE_EXTERN_C PE_API void PE_CALL PE_RunSDLEventLoop();
PE_EXTERN_C PE_API void PE_CALL PE_StopSDLEventLoop();

#endif // !PE_SDL_WRAPPER_H_
