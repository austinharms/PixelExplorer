#include "PxeRefCount.h"

#ifndef PXENGINE_GLASSET_H_
#define PXENGINE_GLASSET_H_
namespace pxengine {
	namespace nonpublic {
		class NpEngineBase;
	}

	enum class PxeGLAssetStatus : uint8_t
	{
		UNINITIALIZED = 0,
		UNINITIALIZING,
		PENDING_UNINITIALIZATION,
		PENDING_INITIALIZATION,
		INITIALIZING,
		INITIALIZED,
		ERROR,
	};

	class PxeGLAsset : public PxeRefCount
	{
	public:
		PxeGLAsset();
		virtual ~PxeGLAsset();
		PxeGLAssetStatus getAssetStatus() const;

	protected:
		// called internally by the engine, all OpenGL object should be created in this function
		// note: you can assume there is a valid OpenGl context bound on the calling thread
		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		virtual void initializeGl() = 0;

		// called internally by the engine, all OpenGL objects created must be cleaned up in this call
		// note: you can assume there is a valid OpenGl context bound on the calling thread
		// ENGINE USE ONLY DO NOT CALL THIS FUNCTION
		virtual void uninitializeGl() = 0;

		// all relevant OpenGL objects created in initializeGl should be bound
		// note: you must bind a valid OpenGl context before calling this function 
		virtual void bind() = 0;

		// all OpenGL object bound in bindGl must be unbound
		// note: you must bind a valid OpenGl context before calling this function 
		virtual void unbind() = 0;

		// requests the engine to uninitialize the asset and calls the uninitializeGl method
		// if blocking is true the method will wait for the asset to be uninitialized before returning
		// note this call will not work if the asset is already in a pending state
		void uninitializeAsset(bool blocking = false);

		// requests the engine to initialize the asset and calls the initializeGl method
		// if blocking is true the method will wait for the asset to be initialized before returning
		// note: there must be at least one window created to initialize assets
		// if blocking is true and there is no window create the method will never exit!
		void initializeAsset(bool blocking = false);

		// handles engine cleanup
		// note: if you override this method you must call this at the end of the new method
		void onDelete() override;

		// changes the asset status to ERROR
		// this may only be called if the asset is initialized or initializing
		// note: ERROR state is cleared on uninitialize before uninitializeGl is called
		void setErrorStatus();

	private:
		friend class pxengine::nonpublic::NpEngineBase;
		PxeGLAssetStatus _status;

		// ENGINE USE ONLY
		void initialize();
		// ENGINE USE ONLY
		void uninitialize();
	};
}
#endif // !PXENGINE_GLASSET_H_
