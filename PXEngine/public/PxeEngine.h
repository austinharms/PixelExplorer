#ifndef PXENGINE_H_
#define PXENGINE_H_
PXE_PRIVATE_IMPLEMENTATION_START
#include <unordered_map>

#include "GL/glew.h"
#include "SDL_log.h"
PXE_PRIVATE_IMPLEMENTATION_END
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeLogger.h"
#include "PxeWindow.h"
#include "PxeScene.h"
#include "PxeInputManager.h"
#include "PxFoundation.h"
#include "PxPhysics.h"
#include "cooking/PxCooking.h"
#include "PxeRenderPipeline.h"

namespace pxengine {
	class PxeApplication;

	class PxeEngine : public PxeRefCount, private physx::PxAssertHandler, private physx::PxErrorCallback {
	public:
		// Returns the active PxeEngine instance
		PXE_NODISCARD static PxeEngine& getInstance();

		// Creates and returns a new PxeWindow or nullptr on failure
		// TODO Update/create notes for window creation
		PXE_NODISCARD PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title);

		// Creates and returns a new PxeScene instance with the provided flags or nullptr on failure
		PXE_NODISCARD PxeScene* createScene(PxeSceneFlags flags = PxeSceneFlag::ALL);

		// Returns the input manager for the engine
		PXE_NODISCARD PxeInputManager& getInputManager();

		// Returns the input manager for the engine
		PXE_NODISCARD const PxeInputManager& getInputManager() const;

		// Returns the render pipeline for the engine
		PXE_NODISCARD PxeRenderPipeline& getRenderPipeline();

		// Returns the render pipeline for the engine
		PXE_NODISCARD const PxeRenderPipeline& getRenderPipeline() const;

		// Stops the application at the end of the current update cycle
		void shutdown();

		// Returns if a shutdown is pending aka shutdown was called
		PXE_NODISCARD bool getShutdownPending() const;

		// Returns the time the last frame took in seconds 
		PXE_NODISCARD float getDeltaTime() const;

		// Returns the physx::PxFoundation instance used by the engine
		PXE_NODISCARD physx::PxFoundation& getPhysicsFoundation() const;

		// Returns the physx::PxPhysics instance used by the engine
		PXE_NODISCARD physx::PxPhysics& getPhysicsBase() const;

		// Returns the physx::PxCooking instance used by the engine
		PXE_NODISCARD physx::PxCooking& getPhysicsCooking() const;

		// Returns the physx::PxCooking instance used by the engine
		PXE_NODISCARD PxeLogInterface& getLoggingInterface() const;

		virtual ~PxeEngine();
		PXE_NOCOPY(PxeEngine);

	private:
		static PxeEngine* s_engineInstance;

		PxeEngine(PxeLogInterface& logInterface);
		static constexpr PxeSize STORAGE_SIZE = 2100;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	public:
		void registerWindow(PxeWindow& window);
		void unregisterWindow(PxeWindow& window);
		void removeScene(PxeScene& scene);
		void updateDeltaTime();
		void bindPrimaryGlContext();
		void initializeGlAsset(PxeGLAsset& asset);
		// Note: must be called from the render thread with primary OpenGl context bound
		void forceInitializeGlAsset(PxeGLAsset& asset);
		void uninitializeGlAsset(PxeGLAsset& asset);
		void shutdownEngine();
		void initGlContext();
		PXE_NODISCARD bool createdPrimaryGlContext();
		// acquires windows read lock and returns a map of all windows
		// Note: you must call releaseWindows after to unlock the window read lock
		PXE_NODISCARD const std::unordered_map<uint32_t, PxeWindow*>& acquireWindows();
		void releaseWindows();
		// acquires scenes read lock and returns a list of all scenes
		// Note: you must call releaseScenes after to unlock the scene read lock
		PXE_NODISCARD const std::list<PxeScene*>& acquireScenes();
		void releaseScenes();
		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
		void operator()(const char* exp, const char* file, int line, bool& ignore) override;

	private:
		friend class PxeApplication;
		struct Impl;

		static void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);
		static void sdlLogCallback(void* userdata, int category, SDL_LogPriority priority, const char* message);

		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		void initPhysics();
		void deinitPhysics();
		void initSDL();
		void deinitSDL();
		// Note: must be called from the render thread with primary OpenGl context bound
		void updateAssetProcessing();
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_H_