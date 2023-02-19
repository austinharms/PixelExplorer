#ifndef PXENGINE_GLASSET_H_
#define PXENGINE_GLASSET_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"

namespace pxengine {
	namespace nonpublic {
		class NpEngine;
	}

	// A class for the management of objects that are part of/use OpenGl state
	// Ex: Textures, Element Buffers...
	class PxeGLAsset : public PxeRefCount
	{
	public:
		// A class for the management of objects that are part of/use OpenGl state
		// Ex: Textures, Element Buffers...
		// delayInitialization prevents the default initialization of the asset
		// if this is true you must call initializeAsset to load the asset
		// Note: if the asset is created on a non engine thread (not on the render, update, physics thread), delayInitialization must be true
		PxeGLAsset(bool delayInitialization = false);
		virtual ~PxeGLAsset();

		// Returns the assets current status
		PXE_NODISCARD PxeGLAssetStatus getAssetStatus() const;

		// Requests the engine to initialize the asset
		// Note: this is only needed if the constructor was called with delayInitialization = true
		// Note: this is not immediate and you must check the asset status if it is loaded 
		void initializeAsset();

	protected:
		// This method should create all OpenGl objects for this asset
		// Note: you can assume there is a valid OpenGl context bound on the calling thread
		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		virtual void initializeGl() = 0;

		// This method should delete all OpenGl objects created in initializeGl()
		// Note: you can assume there is a valid OpenGl context bound on the calling thread
		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		virtual void uninitializeGl() = 0;

		// Handles automatic engine resource cleanup
		// Note: if overridden you MUST call this at the end of the overriding method
		void onDelete() override;

		// Changes the asset current status to ERROR
		// Note: this can only be called if the asset status is INITIALIZING or INITIALIZED
		// Note: ERROR status is cleared before uninitializeGl is called
		void setErrorStatus();

	private:
		friend class pxengine::nonpublic::NpEngine;
		PxeGLAssetStatus _status;
		const bool _delayedInitialization;

		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		void initialize();
		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		void uninitialize();
	};
}
#endif // !PXENGINE_GLASSET_H_
