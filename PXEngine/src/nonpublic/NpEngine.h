#ifndef PXENGINE_NONPUBLIC_ENGINE_H_
#define PXENGINE_NONPUBLIC_ENGINE_H_
#include <unordered_map>
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
#include "PxeFontManager.h"
#include "NpFontManager.h"

namespace pxengine {
	namespace nonpublic {
		// TODO add NpEngineBase class description (copy EngineBase description)
		class NpEngine : public PxeEngine, physx::PxAssertHandler, physx::PxErrorCallback
		{
		public:
			//############# PxeEngine API ##################

			PXE_NODISCARD PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) override;
			PXE_NODISCARD PxeScene* createScene() override;
			PXE_NODISCARD PxeShader* loadShader(const std::filesystem::path& path) override;
			PXE_NODISCARD int8_t getVSyncMode() const override;
			PXE_NODISCARD PxeInputManager& getInputManager() const override;
			PXE_NODISCARD float getDeltaTime() const override;
			PXE_NODISCARD physx::PxFoundation* getPhysicsFoundation() const override;
			PXE_NODISCARD physx::PxPhysics* getPhysicsBase() const override;
			PXE_NODISCARD physx::PxCooking* getPhysicsCooking() const override;
			PXE_NODISCARD PxeFontManager& getFontManager() const override;
			void setVSyncMode(int8_t mode) override;
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
			void uninitializeGlAsset(PxeGLAsset& asset);
			void initializeWindow(NpWindow& window);
			void uninitializeWindow(NpWindow& window);
			PXE_NODISCARD bool getShutdownFlag() const;
			PXE_NODISCARD PxeLogInterface& getLogInterface();
			PXE_NODISCARD NpInputManager& getNpInputManager() const;
			PXE_NODISCARD NpFontManager& getNpFontManager() const;
			void setDeltaTime(float dt);
			void removeShader(const std::filesystem::path& path);
			const std::unordered_map<uint32_t, NpWindow*>& getWindows();
			void newFrame(NpWindow& window);
			void renderFrame(NpWindow& window, PxeRenderPass pass);
			void renderGui(NpWindow& window);
			void swapFramebuffer(NpWindow& window);
			void bindPrimaryGlContext();
			void acquireWindowsReadLock();
			void releaseWindowsReadLock();
			void acquireWindowsWriteLock();
			void releaseWindowsWriteLock();
			void shutdownEngine();

		private:
			static NpEngine* s_instance;
			const char* EXTENDED_WINDOW_DATA_NAME = "PXE_NP_WINDOW_DATA";
			static void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);

			void initPhysics();
			void deinitPhysics();
			void initSDL();
			void deinitSDL();
			void initPrimaryGlContext();
			void forceAssetInit(PxeGLAsset& asset);

			std::shared_mutex _windowsMutex;
			std::mutex _shaderMutex;
			std::mutex _assetMutex;
			// map of all the NpWindows with SDL_Windows that exist by SDL window id
			std::unordered_map<uint32_t, NpWindow*> _sdlWindows;
			std::unordered_map<std::filesystem::path, NpShader*> _shaderCache;
			std::list<PxeGLAsset*> _assetQueue;
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
			NpFontManager* _fontManager;
			SDL_Window* _primarySDLWindow;
			SDL_GLContext _primaryGlContext;
			NpWindow* _activeMouseWindow;
			NpWindow* _activeKeyboardWindow;
			void* _guiBackend;
			uint16_t _guiBackendReferenceCount;
			float _deltaTime;
			int8_t _vsyncMode;
			bool _createdWindow;
			bool _shutdownFlag;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_
