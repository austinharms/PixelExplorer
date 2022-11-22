#include "PxeRefCount.h"

#ifndef PXENGINE_GLASSET_H_
#define PXENGINE_GLASSET_H_
namespace pxengine::rendering {
	class PxeGLAsset : public PxeRefCount
	{
	public:
		PxeGLAsset();
		virtual ~PxeGLAsset();
		virtual void bind() = 0;
		virtual void unbind() = 0;

	protected:
		// called internally by the engine, all OpenGL object should be created in this function
		// note: you can assume there is a valid OpenGl context bound on the calling thread
		virtual void initializeGl() = 0;

		// called internally by the engine, all OpenGL objects created must be cleaned up in this call
		// note: you can assume there is a valid OpenGl context bound on the calling thread
		virtual void uninitializeGl() = 0;

		// returns true if asset has been "Initialized" aka the initializeGl method was called
		// we assume the asset now has instances of OpenGl objects 
		bool getInitialized() const;

		// returns true if the asset is queued for uninitialization
		// you can queue the object for uninitialization by calling uninitialize
		// note this will also return true if uninitialize was called in blocking mode and is waiting to uninitialize the asset
		bool getQueuedForUninitialization() const;

		// requests the engine to "Uninitialize" the object aka call the uninitializeGl function
		// if block is true the function will block the calling thread until the assets has been uninitialized (and this function will always return true)
		// returns true if the object has be uninitialized, returns false if it is queued for uninitialization
		bool uninitialize(bool block = false);

	private: 
		bool _initialized;
		bool _uninitializationQueued;

		void onDelete() override;
	};
}
#endif // !PXENGINE_GLASSET_H_
