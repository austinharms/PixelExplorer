#ifndef PXENGINE_NONPUBLIC_ENGINE_H_
#define PXENGINE_NONPUBLIC_ENGINE_H_
#include <unordered_map>
#include <list>
#include <mutex>
#include <shared_mutex>

#include "GL/glew.h"
#include "PxeTypes.h"
#include "PxeLogger.h"
#include "PxeEngine.h"
#include "PxeInputManager.h"
#include "NpInputManager.h"
#include "foundation/PxAssert.h"
#include "foundation/PxErrorCallback.h"
#include "PxFoundation.h"
#include "PxPhysics.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "common/PxTolerancesScale.h"
#include "extensions/PxDefaultAllocator.h"
#include "cooking/PxCooking.h"
#include "pvd/PxPvdTransport.h"
#include "pvd/PxPvd.h"
#include "PxeWindow.h"
#include "NpWindow.h"
#include "PxeShader.h"
#include "NpShader.h"
#include "PxeScene.h"
#include "imgui.h"
#include "SDL_log.h"
#include "NpRenderPipeline.h"

namespace pxengine {
	namespace nonpublic {
		// TODO add NpEngineBase class description (copy EngineBase description)
		class NpEngine : public PxeEngine, physx::PxAssertHandler, physx::PxErrorCallback
		{
		public:
			//############# PxeEngine API ##################

			PXE_NODISCARD PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) override;
			PXE_NODISCARD PxeScene* createScene() override;
			PXE_NODISCARD PxeInputManager& getInputManager() const override;
			PXE_NODISCARD float getDeltaTime() const override;
			PXE_NODISCARD physx::PxFoundation& getPhysicsFoundation() const override;
			PXE_NODISCARD physx::PxPhysics& getPhysicsBase() const override;
			PXE_NODISCARD physx::PxCooking& getPhysicsCooking() const override;
			PXE_NODISCARD PxeRenderPipeline& getRenderPipeline() const override;
			PXE_NODISCARD bool getShutdownFlag() const override;
			void shutdown() override;


			//############# PxErrorCallback API ##################

			void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;


			//############# PxAssertHandler API ##################

			void operator()(const char* exp, const char* file, int line, bool& ignore) override;


			//############# PRIVATE API ##################

			static PXE_NODISCARD NpEngine& getInstance();

			NpEngine(PxeLogInterface& logHandler);
			virtual ~NpEngine();
			NpEngine(const NpEngine& other) = delete;
			NpEngine operator=(const NpEngine& other) = delete;
			void processAssetQueue();
			void initializeGlAsset(PxeGLAsset& asset);
			// Note: must be called from the render thread with primary OpenGl context bound
			void forceInitializeGlAsset(PxeGLAsset& asset);
			void uninitializeGlAsset(PxeGLAsset& asset);
			void registerWindow(NpWindow& window);
			void unregisterWindow(NpWindow& window);
			void removeScene(NpScene& scene);
			PXE_NODISCARD PxeLogInterface& getLogInterface();
			PXE_NODISCARD NpInputManager& getNpInputManager() const;
			PXE_NODISCARD NpRenderPipeline& getNpRenderPipeline() const;
			PXE_NODISCARD bool hasPrimaryGlContext() const;
			void setDeltaTime(float dt);
			const std::unordered_map<uint32_t, NpWindow*>& getWindows();
			const std::list<NpScene*>& getScenes();
			void bindPrimaryGlContext();
			void acquireWindowsReadLock();
			void releaseWindowsReadLock();
			void acquireScenesReadLock();
			void releaseScenesReadLock();
			void shutdownEngine();
			void initGlContext();

		private:
			static NpEngine* s_instance;
			static void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);
			static void sdlLogOutput(void* userdata, int category, SDL_LogPriority priority, const char* message);

			void initPhysics();
			void deinitPhysics();
			void initSDL();
			void deinitSDL();

			std::shared_mutex _windowsMutex;
			// map of all the NpWindows with SDL_Windows that exist by SDL window id
			std::unordered_map<uint32_t, NpWindow*> _sdlWindows;

			std::mutex _assetMutex;
			std::list<PxeGLAsset*> _assetQueue;

			std::shared_mutex _sceneMutex;
			std::list<NpScene*> _scenes;

			physx::PxAssertHandler& _defaultPhysAssertHandler;
			physx::PxFoundation* _physFoundation;
#ifdef PXE_DEBUG
			physx::PxPvd* _physPVD;
			physx::PxPvdTransport* _physPVDTransport;
#endif
			physx::PxPhysics* _physPhysics;
			physx::PxCooking* _physCooking;
			physx::PxDefaultCpuDispatcher* _physDefaultDispatcher;
			physx::PxDefaultAllocator _physAllocator;
			physx::PxTolerancesScale _physScale;
			PxeLogInterface& _logInterface;
			NpInputManager* _inputManager;
			NpRenderPipeline* _renderPipeline;
			SDL_Window* _primarySDLWindow;
			SDL_GLContext _primaryGlContext;
			float _deltaTime;
			bool _shutdownFlag;
			bool _windowQueueForCreation;
			bool _renderInit;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_
