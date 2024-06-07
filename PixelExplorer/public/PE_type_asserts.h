// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#ifndef PE_TYPE_ASSERTS_H_
#define PE_TYPE_ASSERTS_H_
#include <type_traits>
#include "PE_defines.h"

// Produces a static assert if A and B are different types
#define PE_TYPE_ASSERT(A, B) PE_STATIC_ASSERT((std::is_same<A,B>::value), PE_TEXT("Type "#A" was not the expected type "#B))

// Produces a static assert if DERIVED does not inherit from BASE
#define PE_INHERIT_ASSERT(BASE, DERIVED) PE_STATIC_ASSERT((std::is_base_of<BASE, DERIVED>::value), PE_TEXT(#DERIVED" must inherit from "#BASE))

// Produces a static assert if A is not trivially copyable
#define PE_TRIVIAL_COPY_ASSERT(A) PE_STATIC_ASSERT((std::is_trivially_copyable<A>::value), PE_TEXT(#A" must be trivially copyable"))
#endif // !PE_TYPE_ASSERTS_H_
