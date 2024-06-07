// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_DEFINES_H_
#define PE_DEFINES_H_
#define PE_OVERRIDE override
#define PE_EXTERN extern
#define PE_EXTERN_C extern "C"
#define PE_FINAL final
#define PE_NOEXCEPT noexcept
#define PE_TEXT_INTERNAL(text) u8##text
#define PE_TEXT(text) PE_TEXT_INTERNAL(text)
#define PE_GENERIC_TEXT_INTERNAL(text) #text
#define PE_GENERIC_TEXT(text) PE_GENERIC_TEXT_INTERNAL(text)

// TODO: This only works on MSVC
#if defined(_MSC_VER)
#define PE_EXPORT_FROM_LIB __declspec(dllexport)
#define PE_IMPORT_FROM_LIB __declspec(dllimport)
#define PE_NORETURN __declspec(noreturn)
#define PE_INLINE __inline
#define PE_FORCEINLINE __forceinline
#define PE_NODISCARD [[nodiscard]]
#define PE_NOINLINE __declspec(noinline)
#define PE_CALL __cdecl
#define PE_FUNCTION_NAME PE_TEXT(__FUNCTION__)
#define PE_FILE_LINE __LINE__
#define PE_FILE_NAME PE_TEXT(__FILE__)
#else
#error Unknow compiler
#endif

#define PE_STATIC_ASSERT(exp, msg) static_assert(exp, msg)
#define PE_ARRAY_LEN(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define PE_NOCOPY(Class) Class(const Class&) = delete; Class& operator=(const Class&) = delete;
#ifdef PE_EXPORT_API
#define PE_API PE_EXPORT_FROM_LIB
#else
#define PE_API PE_IMPORT_FROM_LIB
#endif

#if !defined(NDEBUG) || defined(DEBUG)
#define PE_DEBUG 1
#else
#define PE_DEBUG 0
#endif
#endif // !PE_DEFINES_H_
