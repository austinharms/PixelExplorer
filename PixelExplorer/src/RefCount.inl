#include <stdint.h>

namespace pixelexplorer {
	RefCount::RefCount() : _count(1) {}

	RefCount::~RefCount() {}

	void RefCount::grab() {
		++_count;
	}

	bool RefCount::drop() {
		if (--_count == 0) {
			delete this;
			return true;
		}

		return false;
	}

	uint32_t RefCount::getRefCount() const {
		return _count;
	}
}