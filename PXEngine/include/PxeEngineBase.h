#include <stdint.h>
#include <filesystem>

#include "PxeRefCount.h"
#include "PxeLogger.h"
#include "PxeWindow.h"
#include "PxeShader.h"

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

		// loads a PxeShader from path
		// returns nullptr if there was an error
		// note even if nullptr was not returned the PxeShader may have an error
		// also all shaders are cached and there should only ever be one PxeShader with the same path at a time
		virtual PxeShader* loadShader(const std::filesystem::path& path) = 0;

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
