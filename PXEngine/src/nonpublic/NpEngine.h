#ifndef PXENGINE_NONPUBLIC_ENGINE_H_
#define PXENGINE_NONPUBLIC_ENGINE_H_
#include <unordered_map>

#include "GL/glew.h"
#include "PxeTypes.h"
#include "PxeLogger.h"
#include "PxeEngine.h"
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
			void setVSyncMode(int8_t mode) override;
			PXE_NODISCARD int8_t getVSyncMode() const override;

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
			void processEvents();
			void processAssetQueue();
			void initializeGlAsset(PxeGLAsset& asset);
			void uninitializeGlAsset(PxeGLAsset& asset);
			PXE_NODISCARD bool getShutdownFlag() const;
			void removeShader(const std::filesystem::path& path);
			void removeWindow(NpWindow& window);
			PXE_NODISCARD PxeLogInterface& getLogInterface();
			PXE_NODISCARD SDL_Window* createSDLWindow(NpWindow& window);
			void destroySDLWindow(SDL_Window* sdlWindow, NpWindow& window);
			PXE_NODISCARD ImGuiContext* createGuiContext(NpWindow& window);
			void destroyGuiContext(ImGuiContext* context, NpWindow& window);
			void shutdownEngine();
			
			const std::unordered_map<uint32_t, NpWindow*>& getWindows();
			void newFrame(NpWindow& window);
			void renderFrame(NpWindow& window);
			void renderGui(NpWindow& window);
			void swapFramebuffer(NpWindow& window);
			void bindPrimaryGlContext();

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

			// map of all the NpWindows with SDL_Windows that exist by SDL window id
			std::unordered_map<uint32_t, NpWindow*> _sdlWindows;
			std::unordered_map<std::filesystem::path, NpShader*> _shaderCache;
			// TODO Make this a thread safe queue
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
			NpWindow* _primaryWindow;
			SDL_GLContext _primaryGlContext;
			uint32_t _activeMouseWindowId;
			uint32_t _activeKeyboardWindowId;
			ImFontAtlas _guiFontAtlas;
			void* _guiBackend;
			uint16_t _guiBackendReferenceCount;
			// Hack as we need the primary SDL window until right before shutdown to clean up PxeGlAssets, so store it here
			void* _shutdownFlag;
			int8_t _vsyncMode;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_
