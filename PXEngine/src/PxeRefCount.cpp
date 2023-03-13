#include "PxeRefCount.h"

namespace pxengine {
#ifdef PXE_DEBUG
	std::atomic<PxeSize> PxeRefCount::s_totalCount = 0;
#endif // PXE_DEBUG

	PxeRefCount::PxeRefCount() : _refCount(1) { ++s_totalCount; }

	PxeRefCount::~PxeRefCount() { --s_totalCount; }

	PXE_NODISCARD PxeSize PxeRefCount::getRefCount()
	{
		return _refCount;
	}

	PxeSize PxeRefCount::getPxeRefCountInstanceCount()
	{
#ifdef PXE_DEBUG
		return s_totalCount;
#else
		return 0;
#endif
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