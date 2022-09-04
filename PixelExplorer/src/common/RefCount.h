#include <atomic>
#include <stdint.h>

#ifndef PIXELEXPLORE_REFCOUNT_H_
#define PIXELEXPLORE_REFCOUNT_H_
namespace pixelexplorer {
	class RefCount
	{
	public:
		inline RefCount();
		inline virtual ~RefCount();
		inline virtual void grab();
		inline virtual bool drop();
		inline uint32_t getRefCount() const;

	private:
		std::atomic<uint32_t> _count;
	};
}

#include "RefCount.inl"
#endif // !PIXELEXPLORE_REFCOUNT_H_
