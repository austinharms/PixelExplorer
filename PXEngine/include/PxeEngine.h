
// Enable various debugging checks and tests 
//#define PXE_DEBUG 1

// Disables the logging of assertions to std out in PxeDefaultAssertHandler
//#define PXE_DISABLE_DEFAULT_ASSERT_LOG

// Disables the calling of abort in PxeDefaultAssertHandler
//#define PXE_DISABLE_DEFAULT_ASSERT_ABORT

#ifndef PXENGINE_H_
#define PXENGINE_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeApplicationInterface.h"
#include "PxeLogger.h"
#include "PxeWindow.h"
#include "PxeScene.h"
#include "PxeShader.h"
#include "PxeInputManager.h"

namespace pxengine {
	class PxeEngine : public PxeRefCount {
	public:
		// Creates and returns a new PxeWindow or nullptr on failure
		// TODO Update/create notes for window creation
		virtual PXE_NODISCARD PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) = 0;

		// Creates and returns a new PxeScene or nullptr on failure
		virtual PXE_NODISCARD PxeScene* createScene() = 0;

		// Returns the input manager for the engine
		virtual PXE_NODISCARD PxeInputManager& getInputManager() const = 0;

		// Set VSync mode for all windows
		// Modes:
		//	0 for immediate updates
		//	1 for updates synchronized with
		//	-1 for adaptive VSync
		// Note: the default is 0
		virtual void setVSyncMode(int8_t mode) = 0;

		// Returns VSync mode for all windows
		virtual PXE_NODISCARD int8_t getVSyncMode() const = 0;

		// Loads and returns PxeShader or nullptr on failure 
		// Note: returned shaders still need to be validated before use
		// Note: all PxeShader are cached by {path} and are not guaranteed to be reloaded from disk
		virtual PXE_NODISCARD PxeShader* loadShader(const std::filesystem::path& path) = 0;

		// Stops the application at the end of the current update cycle
		virtual void shutdown() = 0;

		PxeEngine() = default;
		virtual ~PxeEngine() = default;
		PxeEngine(const PxeEngine& other) = delete;
		PxeEngine operator=(const PxeEngine& other) = delete;
	};

	// Returns the PxeEngine instance
	extern PXE_NODISCARD PxeEngine& pxeGetEngine();

	// TODO Add function description
	extern void pxeRunApplication(PxeApplicationInterface& application, PxeLogInterface& logInterface);
}
#endif // !PXENGINE_H_