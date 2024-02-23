#include "PE_log.h"
#include <cstdlib>

void PE_InitLog()
{
	SDL_LogSetPriority(PE_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(PE_LOG_CATEGORY_ASSERT, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(PE_LOG_CATEGORY_CORE, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(PE_LOG_CATEGORY_EVENT, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(PE_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetPriority(PE_LOG_CATEGORY_TEST, SDL_LOG_PRIORITY_VERBOSE);
	PE_STATIC_ASSERT(PE_LOG_CATEGORY_CUSTOM == 35, PE_TEXT("PE_log.cpp PE_LOG_CATEGORY_* count changed update PE_InitLog"));
	// Log here to ensure all SDL thread synchronization mechanisms are created before returning
	PE_LogInfo(PE_LOG_CATEGORY_CORE, "PixelExplorer Log Begin");
}

void PE_LogCriticalAbort() {
	std::abort();
}
