#include "PE_referenced.h"
namespace pe {
	void Referenced::Grab()
	{
		SDL_AtomicIncRef(&atomic_reference_count_);
	}

	void Referenced::Drop()
	{
		if (SDL_AtomicDecRef(&atomic_reference_count_)) {
			OnDrop();
		}
	}

	PE_NODISCARD Referenced::Count Referenced::GetReferenceCount() const
	{
		return SDL_AtomicGet(const_cast<SDL_AtomicInt*>(&atomic_reference_count_));
	}

	Referenced::Referenced()
	{
		SDL_AtomicSet(&atomic_reference_count_, 1);
	}
}
