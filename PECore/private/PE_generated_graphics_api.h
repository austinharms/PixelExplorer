// THIS IS AN AUTO GENERATED FILE DO NOT EDIT
// This file is included multiple times don't add an include guard
#ifndef PE_GENERATED_GRAPHICS_API
#error PE_GENERATED_GRAPHICS_API must be defined when including this file
#endif // !PE_GENERATED_GRAPHICS_API

PE_GENERATED_GRAPHICS_API(int, InitSystem, (), (), return)
PE_GENERATED_GRAPHICS_API(void, QuitSystem, (), (), )
PE_GENERATED_GRAPHICS_API(SDL_Window*, CreateWindow, (char* title, int width, int height, Uint32 flags), (title, width, height, flags), return)
PE_GENERATED_GRAPHICS_API(void, DestroyWindow, (SDL_Window* window), (window), )
PE_GENERATED_GRAPHICS_API(Shader*, LoadShader, (const char* name), (name), return)
PE_GENERATED_GRAPHICS_API(void, UnloadShader, (Shader* shader), (shader), )
