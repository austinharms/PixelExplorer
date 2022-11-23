#include "PxeGLAsset.h"

#include "NpLogger.h"
#include "NpEngineBase.h"

namespace pxengine {
	PxeGLAsset::PxeGLAsset()
	{
		_initialized = false;
		_uninitializationQueued = false;
	}

	PxeGLAsset::~PxeGLAsset()
	{
		if (_initialized) {
			PXE_WARN("PxeGLAsset not uninitialized before destructor was called, uninitializing asset");
			uninitializeAsset(true);
		}
	}

	bool PxeGLAsset::getInitialized() const
	{
		return _initialized;
	}

	bool PxeGLAsset::getQueuedForUninitialization() const
	{
		return _uninitializationQueued;
	}

	bool PxeGLAsset::uninitializeAsset(bool block)
	{
		if (!_initialized) {
			PXE_WARN("uninitializeAsset called on uninitialized PxeGlAsset");
			return true;
		}

		_uninitializationQueued = true;
		return nonpublic::NpEngineBase::getInstance().uninitializeGlAsset(*this, block);
	}

	void PxeGLAsset::onDelete()
	{
		if (_initialized)
			uninitializeAsset(true);
	}

	void PxeGLAsset::initialize()
	{
		if (_initialized) {
			PXE_WARN("initialize called on initialized PxeGlAsset");
			return;
		}

		initializeGl();
		_initialized = true;
	}

	void PxeGLAsset::uninitialize()
	{
		if (!_initialized) {
			PXE_WARN("uninitialize called on uninitialized PxeGlAsset");
			return;
		}

		uninitializeGl();
		_uninitializationQueued = false;
		_initialized = false;
	}
}