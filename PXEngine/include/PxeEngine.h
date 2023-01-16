
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

namespace pxengine {
	class PxeEngine : public PxeRefCount {
	public:
		// Creates and returns a new PxeWindow or nullptr on failure
		// TODO Update/create notes for window creation
		virtual PXE_NODISCARD PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title, PxeWindowId windowId) = 0;

		// Returns the PxeWindow with id {windowId} or nullptr if no window exist
		virtual PXE_NODISCARD PxeWindow* getWindow(PxeWindowId windowId) = 0;

		// Creates and returns a new PxeScene or nullptr on failure
		virtual PXE_NODISCARD PxeScene* createScene() = 0;

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