#include "PxeGLAsset.h"

#include "PxeLogger.h"
#include "PxeEngine.h"

namespace pxengine {
	PxeGLAsset::PxeGLAsset(bool delayInitialization) : _delayedInitialization(delayInitialization)
	{
		_status = PxeGLAssetStatus::UNINITIALIZED;
		if (!_delayedInitialization)
			PxeEngine::getInstance().initializeGlAsset(*this);
	}

	PxeGLAsset::~PxeGLAsset()
	{
		if (_status >= PxeGLAssetStatus::INITIALIZED) {
			PXE_ERROR("PxeGLAsset not uninitialized before destructor was called, you must call PxeGLAsset onDelete if it's overridden");
		}
		// this should never happen as the engine grabs things that are pending
		else if (_status != PxeGLAssetStatus::UNINITIALIZED) {
			PXE_ERROR("PxeGlAsset destroyed in pending status, reference counting error!");
		}
	}

	PXE_NODISCARD PxeGLAssetStatus PxeGLAsset::getAssetStatus() const
	{
		return _status;
	}

	void PxeGLAsset::initializeAsset() {
		if (_delayedInitialization) {
			if (_status == PxeGLAssetStatus::UNINITIALIZED) {
				PxeEngine::getInstance().initializeGlAsset(*this);
			}
			else {
				PXE_WARN("Attempted to initialize asset multiple times");
			}
		}
		else {
			PXE_WARN("Attempted to initialize asset when delayInitialization was not set");
		}
	}

	void PxeGLAsset::onDelete()
	{
		if (_status >= PxeGLAssetStatus::INITIALIZED) {
			PxeEngine::getInstance().uninitializeGlAsset(*this);
		}
		// this should never happen as the engine grabs things that are pending
		else if (_status != PxeGLAssetStatus::UNINITIALIZED) {
			PXE_ERROR("Asset dropped in pending status, reference counting error!");
		}
	}

	void PxeGLAsset::setErrorStatus()
	{
		if (_status < PxeGLAssetStatus::INITIALIZING) {
			PXE_WARN("Invalid call to setErrorStatus asset must have an initializing or initialized status");
			return;
		}

		_status = PxeGLAssetStatus::ERROR;
	}

	void PxeGLAsset::initialize()
	{
		if (_status != PxeGLAssetStatus::PENDING_INITIALIZATION) {
			PXE_ERROR("initialize called on when status was not PENDING_INITIALIZATION");
			return;
		}

		_status = PxeGLAssetStatus::INITIALIZING;
		PxeEngine::getInstance().grab();
		initializeGl();
		if (_status != PxeGLAssetStatus::ERROR)
			_status = PxeGLAssetStatus::INITIALIZED;
	}

	void PxeGLAsset::uninitialize()
	{
		if (_status != PxeGLAssetStatus::PENDING_UNINITIALIZATION) {
			PXE_ERROR("uninitialize called on when status was not PENDING_UNINITIALIZATION");
			return;
		}

		_status = PxeGLAssetStatus::UNINITIALIZING;
		uninitializeGl();
		PxeEngine::getInstance().drop();
		_status = PxeGLAssetStatus::UNINITIALIZED;
	}
}