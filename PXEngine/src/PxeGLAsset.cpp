#include "PxeGLAsset.h"

#include "NpLogger.h"

namespace pxengine::rendering {
	PxeGLAsset::PxeGLAsset()
	{
		_initialized = false;
		_uninitializationQueued = false;
	}

	PxeGLAsset::~PxeGLAsset()
	{
		if (_initialized) {
			PXE_WARN("PxeGLAsset not uninitialized before destructor was called, uninitializing asset");
			uninitialize(true);
		}
	}

	void PxeGLAsset::setInitializedFlag()
	{
		_initialized = true;
	}

	const bool PxeGLAsset::getInitializedFlag()
	{
		return _initialized;
	}

	void PxeGLAsset::onDelete()
	{
		PXE_INFO("TODO implement this function");
	}
}