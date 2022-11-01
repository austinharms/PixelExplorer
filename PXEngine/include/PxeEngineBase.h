#include <stdint.h>

#include "PxeRefCount.h"
#include "PxeLogger.h"

#ifndef PXENGINE_ENGINEBASE_H_
#define PXENGINE_ENGINEBASE_H_
namespace pxengine {
	// TODO add EngineBase class description
	class PxeEngineBase : public PxeRefCount
	{
	public:
		// Test Function
		// TODO REMOVE THIS FUNCTION
		virtual uint32_t testFn(uint32_t val) = 0;

	protected:
		virtual ~PxeEngineBase() {}
	};

	// creates and returns an PxeEngineBase instance
	// there should only be one instance per process
	// returns nullptr on error or if an PxeEngineBase instance already exists
	PxeEngineBase* createPXEEngineBase(PxeLogHandler& logHandler);

	// returns the PxeEngineBase instance
	PxeEngineBase& getPXEEngineBase();
}
#endif // !PXENGINE_ENGINEBASE_H_
