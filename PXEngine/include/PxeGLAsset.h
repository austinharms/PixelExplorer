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
		PxeGLAsset();
		virtual ~PxeGLAsset();

		// Returns the assets current status
		PXE_NODISCARD PxeGLAssetStatus getAssetStatus() const;

		// TODO Is this function needed?
		// Requests the engine to uninitialize the asset
		// if blocking is true the method will wait for the asset to be uninitialized before returning
		// Note: this call will not work if the asset is already in a pending state
		//void uninitializeAsset(bool blocking = false);

		// TODO Is this function needed?
		// Requests the engine to initialize the asset
		// if blocking is true the method will wait for the asset to be initialized before returning
		// Note: there must be at least one window created to initialize assets
		// Note: if blocking is true and there is no window create the method will never exit!
		// void initializeAsset(bool blocking = false);

		// This method should bind the OpenGl object to the current state
		// Note: this function MUST be called from the render thread 
		virtual void bind() = 0;

		// TODO Should this restore old state?
		// This method should remove all bound OpenGl objects from state that were bound in bind()
		// Note: this function MUST be called from the render thread 
		virtual void unbind() = 0;

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

		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		void initialize();
		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		void uninitialize();
	};
}
#endif // !PXENGINE_GLASSET_H_
