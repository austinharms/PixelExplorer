#include "PE_barrier.h"

namespace pe::internal {
	Barrier::Barrier(size_t count) :
		threshold_(count),
		current_(count),
		generation_(0) {
	}

	void Barrier::Wait() {
		std::unique_lock lock(mutex_);
		size_t local_gen = generation_;
		if (--current_ == 0) {
			generation_++;
			current_ = threshold_;
			lock.unlock();
			condition_.notify_all();
		}
		else {
			condition_.wait(lock, [this, local_gen] { return local_gen != generation_; });
		}

	}

	void Barrier::Trigger() {
		std::unique_lock lock(mutex_);
		if (--current_ == 0) {
			generation_++;
			current_ = threshold_;
			lock.unlock();
			condition_.notify_all();
		}
	}
}