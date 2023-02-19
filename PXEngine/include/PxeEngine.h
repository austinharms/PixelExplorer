
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
#include "PxeInputManager.h"
#include "PxFoundation.h"
#include "PxPhysics.h"
#include "cooking/PxCooking.h"
#include "PxeRenderPipeline.h"

namespace pxengine {
	class PxeEngine : public PxeRefCount {
	public:
		// Creates and returns a new PxeWindow or nullptr on failure
		// TODO Update/create notes for window creation
		PXE_NODISCARD virtual PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) = 0;

		// Creates and returns a new PxeScene or nullptr on failure
		PXE_NODISCARD virtual PxeScene* createScene() = 0;

		// Returns the input manager for the engine
		PXE_NODISCARD virtual PxeInputManager& getInputManager() const = 0;

		// Returns the render pipeline for the engine
		PXE_NODISCARD virtual PxeRenderPipeline& getRenderPipeline() const = 0;

		// Stops the application at the end of the current update cycle
		virtual void shutdown() = 0;

		// Returns if a shutdown is pending
		PXE_NODISCARD virtual bool getShutdownFlag() const = 0;

		// Returns the time the last frame took in seconds 
		PXE_NODISCARD virtual float getDeltaTime() const = 0;

		// Returns the physx PxFoundation instance created and used by the engine
		PXE_NODISCARD virtual physx::PxFoundation& getPhysicsFoundation() const = 0;

		// Returns the physx PxPhysics instance created and used by the engine
		PXE_NODISCARD virtual physx::PxPhysics& getPhysicsBase() const = 0;

		// Returns the physx PxCooking instance created and used by the engine
		PXE_NODISCARD virtual physx::PxCooking& getPhysicsCooking() const = 0;

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