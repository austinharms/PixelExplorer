#include <atomic>
#include <stdint.h>

#ifndef PIXELEXPLORE_REFCOUNT_H_
#define PIXELEXPLORE_REFCOUNT_H_
namespace pixelexplore {
	class RefCount
	{
	public:
		inline RefCount();
		inline virtual ~RefCount();
		inline void grab();
		inline bool drop();
		inline uint32_t getRefCount() const;

	private:
		std::atomic<uint32_t> _count;
	};
}

#include "RefCount.inl"
#endif // !PIXELEXPLORE_REFCOUNT_H_
