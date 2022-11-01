#include "PxeGLAsset.h"

#include "NpLogger.h"

namespace pxengine::rendering {
	PxeGLAsset::PxeGLAsset()
	{
		_initialized = false;
	}

	PxeGLAsset::~PxeGLAsset()
	{
		if (_initialized)
			PXE_WARN("GLAsset not uninitialized, this can cause memory leaks");
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