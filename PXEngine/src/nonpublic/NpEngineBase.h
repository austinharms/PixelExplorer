#include <mutex>
#include <unordered_map>
#include <thread>
#include <list>
#include <filesystem>

#include "PxeEngineBase.h"
#include "PxPhysicsAPI.h"
#include "PxeLogger.h"
#include "GL/glew.h"
#include "SDL.h"
#include "NpWindow.h"
#include "PxeScene.h"
#include "PxeGLAsset.h"
#include "NpShader.h"

#ifndef PXENGINE_NONPUBLIC_ENGINEBASE_H_
#define PXENGINE_NONPUBLIC_ENGINEBASE_H_
namespace pxengine::nonpublic {
	// TODO add NpEngineBase class description (copy EngineBase description)
	class NpEngineBase : public PxeEngineBase, physx::PxAssertHandler, physx::PxErrorCallback
	{
	public:
		//############# PUBLIC API ##################

		PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) override;

		PxeScene* createScene() override;

		PxeShader* loadShader(const std::filesystem::path& path) override;

		void shutdown() override;


		//############# PRIVATE API ##################

		static NpEngineBase* createInstance(PxeLogInterface& logInterface);

		static NpEngineBase& getInstance();

		virtual ~NpEngineBase();

		void destroyWindow(NpWindow& window);

		void acquireGlContextLock();

		void releaseGlContextLock();

		bool acquiredGlContext() const;

		bool acquiredGlContext(const NpWindow& window) const;

		void acquireGlContext(NpWindow& window);

		void releaseGlContext(NpWindow& window);

		void pollEvents();

		void uninitializeGlAsset(PxeGLAsset& asset, bool blocking = false);

		void initializeGlAsset(PxeGLAsset& asset, bool blocking = false);

		bool getShutdown() const;

		void removeShaderFromCache(const std::filesystem::path& path);


		//############# PxErrorCallback API ##################

		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;


		//############# PxAssertHandler API ##################

		void operator()(const char* exp, const char* file, int line, bool& ignore) override;

	private:
		static NpEngineBase* s_instance;

		NpEngineBase(PxeLogInterface& logHandler);
		void initPhys();
		void deinitPhys();
		void initSDL();
		void deinitSDL();
		void updateQueuedAssets();

		// map of all the windows that exist by SDL window id
		std::unordered_map<uint32_t, NpWindow*> _windows;
		std::unordered_map<std::filesystem::path, NpShader*> _shaders;
		std::list<PxeGLAsset*> _assetQueue;
		std::recursive_mutex _glMutex;
		std::mutex _glNextMutex;
		NpWindow* _activeWindow;
		std::thread::id _boundContextThread;
		physx::PxAssertHandler& _defaultPhysAssertHandler;
		physx::PxFoundation* _physFoundation;
		physx::PxPvd* _physPVD;
		physx::PxPvdTransport* _physPVDTransport;
		physx::PxPhysics* _physPhysics;
		physx::PxCooking* _physCooking;
		physx::PxCpuDispatcher* _physDefaultDispatcher;
		physx::PxDefaultAllocator _physAllocator;
		physx::PxTolerancesScale _physScale;
		SDL_GLContext _sdlGlContext;
		uint32_t _activeMouseWindowId;
		uint32_t _activeKeyboardWindowId;
		uint8_t _glContextDepth;
		bool _engineInit;
	};
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_
