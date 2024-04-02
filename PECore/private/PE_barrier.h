#ifndef PE_BARRIER
#define PE_BARRIER
#include "PE_defines.h"
#include <mutex>
#include <condition_variable>

namespace pecore {
	// A reusable thread synchronisation object
	class Barrier {
	public:
		// Count is how many calls to Wait() are required before the Barrier unblocks
		Barrier(size_t count);
		// Waits for count calls to Wait() before returning
		void Wait();
		// Acts as a call to Wait() that does not block
		void Trigger();

	private:
		std::mutex mutex_;
		std::condition_variable condition_;
		size_t threshold_;
		size_t current_;
		size_t generation_;
	};
}
#endif // !PE_BARRIER