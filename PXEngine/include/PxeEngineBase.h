#include <stdint.h>

#include "PxeRefCount.h"
#include "PxeLogger.h"
#include "PxeWindow.h"

#ifndef PXENGINE_ENGINEBASE_H_
#define PXENGINE_ENGINEBASE_H_
namespace pxengine {
	// TODO add EngineBase class description
	class PxeEngineBase : public PxeRefCount
	{
	public:
		// returns a new PxeWindow object or nullptr if there was an error creating the window
		// note the PxeWindow shares it GlContext with all other windows 
		virtual PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) = 0;

		// returns a new PxeScene object or nullptr if there was an error 
		virtual PxeScene* createScene() = 0;

		// used to destroy the engine base
		// this will log an error if resources are still in use
		// this also drops the engine base so do not call drop after calling this function
		virtual void shutdown() = 0;

		virtual ~PxeEngineBase() {}

		PxeEngineBase() = default;
		PxeEngineBase(const PxeEngineBase& other) = delete;
		PxeEngineBase operator=(const PxeEngineBase& other) = delete;
	};

	// creates and returns an PxeEngineBase instance
	// there should only be one instance per process
	// returns nullptr on error or if an PxeEngineBase instance already exists
	PxeEngineBase* createPXEEngineBase(PxeLogHandler& logHandler);

	// returns the PxeEngineBase instance
	PxeEngineBase& getPXEEngineBase();
}
#endif // !PXENGINE_ENGINEBASE_H_
