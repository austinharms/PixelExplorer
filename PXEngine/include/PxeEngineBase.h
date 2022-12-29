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
		// note the PxeWindow shares it Gl Context with all other windows 
		virtual PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) = 0;

		// returns a new PxeScene object or nullptr if there was an error 
		virtual PxeScene* createScene() = 0;

		// used to destroy the PxeEngineBase and calls drop
		// note this will log an error if resources are still using the PxeEngineBase 
		virtual void shutdown() = 0;

		virtual ~PxeEngineBase() {}

		PxeEngineBase() = default;
		PxeEngineBase(const PxeEngineBase& other) = delete;
		PxeEngineBase operator=(const PxeEngineBase& other) = delete;
	};

	// creates and returns an PxeEngineBase instance
	// returns nullptr on error or if an PxeEngineBase instance already exists
	// note there should only be one instance per process
	PxeEngineBase* createPXEEngineBase(PxeLogInterface& logInterface);

	// returns the PxeEngineBase instance
	// note calls assert if no instance exists
	PxeEngineBase& getPXEEngineBase();
}
#endif // !PXENGINE_ENGINEBASE_H_
