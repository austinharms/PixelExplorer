// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include "PE_referenced.h"
#include "SDL3/SDL_atomic.h"

namespace pe {
	PE_STATIC_ASSERT(sizeof(SDL_AtomicInt) == sizeof(Referenced::RefCount), PE_TEXT("Referenced atomic size invalid"));
	void Referenced::Grab()
	{
		SDL_AtomicIncRef(reinterpret_cast<SDL_AtomicInt*>(&atomic_reference_count_));
	}

	void Referenced::Drop()
	{
		if (SDL_AtomicDecRef(reinterpret_cast<SDL_AtomicInt*>(&atomic_reference_count_))) {
			OnDrop();
		}
	}

	PE_NODISCARD Referenced::RefCount Referenced::GetRefCount() const
	{
		return SDL_AtomicGet(const_cast<SDL_AtomicInt*>(reinterpret_cast<const SDL_AtomicInt*>(&atomic_reference_count_)));
	}

	Referenced::Referenced()
	{
		SDL_AtomicSet(reinterpret_cast<SDL_AtomicInt*>(&atomic_reference_count_), 1);
	}
}
