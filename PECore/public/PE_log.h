#ifndef PE_LOG_H_
#define PE_LOG_H_
#include "PE_defines.h"
#include "SDL_log.h"

typedef enum
{
    PE_LOG_CATEGORY_ERROR = SDL_LOG_CATEGORY_CUSTOM,
    PE_LOG_CATEGORY_ASSERT,
    PE_LOG_CATEGORY_CORE,
    PE_LOG_CATEGORY_EVENT,
    PE_LOG_CATEGORY_RENDER,
    PE_LOG_CATEGORY_TEST,
    PE_LOG_CATEGORY_RESERVED1,
    PE_LOG_CATEGORY_RESERVED2,
    PE_LOG_CATEGORY_RESERVED3,
    PE_LOG_CATEGORY_RESERVED4,
    PE_LOG_CATEGORY_RESERVED5,
    PE_LOG_CATEGORY_RESERVED6,
    PE_LOG_CATEGORY_RESERVED7,
    PE_LOG_CATEGORY_RESERVED8,
    PE_LOG_CATEGORY_RESERVED9,
    PE_LOG_CATEGORY_RESERVED10,
    PE_LOG_CATEGORY_CUSTOM
} PE_LogCategory;

PE_EXTERN_C PE_API void PE_CALL PE_InitLog();
// Aborts the application
PE_NORETURN PE_EXTERN_C PE_API void PE_CALL PE_LogCriticalAbort();

#define PE_LogMessage SDL_LogMessage
#define PE_LogVerbose SDL_LogVerbose
#define PE_LogDebug SDL_LogDebug
#define PE_LogInfo SDL_LogInfo
#define PE_LogWarn SDL_LogWarn
#define PE_LogError SDL_LogError
#define PE_LogCritical(category, fmt, ...) do { SDL_LogCritical(category, fmt, __VA_ARGS__); PE_LogCriticalAbort(); } while(false)

#define PE_STATIC_ASSERT(exp, msg) static_assert(exp, msg)
#define PE_ASSERT(condition, fmt, ...) do { if (!(condition)) { PE_LogCritical(PE_LOG_CATEGORY_ASSERT, fmt, __VA_ARGS__); } } while(false)

#define PE_TEXT_INTERNAL(text) text
#define PE_TEXT(text) PE_TEXT_INTERNAL(text)

#if PE_DEBUG
#define PE_DEBUG_ASSERT PE_ASSERT
#else
#define PE_DEBUG_ASSERT ((void)0)
#endif
#endif // !PE_LOG_H_