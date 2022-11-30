#include "PxeRefCount.h"

namespace pxengine {
	PxeRefCount::PxeRefCount() : _refCount(1) {}

	PxeRefCount::~PxeRefCount() {}

	size_t PxeRefCount::getRefCount()
	{
		return _refCount;
	}

	void PxeRefCount::grab()
	{
		++_refCount;
	}

	bool PxeRefCount::drop()
	{
		if (--_refCount == 0) {
			onDelete();
			if (_refCount == 0) {
				delete this;
				return true;
			}
		}

		return false;
	}
}