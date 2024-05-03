#ifndef PE_DEFINES_H_
#define PE_DEFINES_H_
// Make PE defines consistant we SDL defines
#include "SDL_stdinc.h"
#define PE_NORETURN SDL_NORETURN
#define PE_INLINE SDL_INLINE
#define PE_FORCEINLINE SDL_FORCE_INLINE

#define PE_API
#define PE_NODISCARD [[nodiscard]]
#define PE_CALL __cdecl
#define PE_NOCOPY(Class) Class(const Class&) = delete; Class& operator=(const Class&) = delete;
#define PE_OVERRIDE override
#define PE_NOINLINE __declspec(noinline)
#define PE_FINAL final
#define PE_WEAK __attribute__((weak))
#define PE_NOEXCEPT noexcept
#define PE_EXCEPT noexcept(false)
#define PE_MAYBE_UNUSED [[maybe_unused]]
#define PE_STATIC_ASSERT(exp, msg) static_assert(exp, msg)
#define PE_ARRAY_LEN(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define PE_TEXT_INTERNAL(text) text
#define PE_TEXT(text) PE_TEXT_INTERNAL(text)
#define PE_STATIC_ASSERT(exp, msg) static_assert(exp, msg)
#define PE_EXTERN_C extern "C"

#if !defined(NDEBUG) || defined(DEBUG)
#define PE_DEBUG 1
#else
#define PE_DEBUG 0
#endif

#endif // !PE_DEFINES_H_
