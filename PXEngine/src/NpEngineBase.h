#include <mutex>
#include <unordered_map>
#include <thread>
#include <list>

#include "PxeEngineBase.h"
#include "PxeLogger.h"
#include "PxPhysicsAPI.h"
#include "GL/glew.h"
#include "SDL.h"
#include "NpWindow.h"
#include "PxeScene.h"
#include "PxeGLAsset.h"

#ifndef PXENGINE_NONPUBLIC_ENGINEBASE_H_
#define PXENGINE_NONPUBLIC_ENGINEBASE_H_
namespace pxengine::nonpublic {
	// TODO add NpEngineBase class description (copy EngineBase description)
	class NpEngineBase : public PxeEngineBase, PxeLogHandler, physx::PxAssertHandler, physx::PxErrorCallback
	{
	public:
		//############# PUBLIC API ##################

		PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) override;

		PxeScene* createScene() override;

		//############# PRIVATE API ##################

		// creates and returns an NpEngineBase instance
		// there should only be one instance per process
		// returns nullptr on error or if an NpEngineBase instance already exists
		static NpEngineBase* createInstance(PxeLogHandler& logHandler);

		// returns the NpEngineBase instance
		static NpEngineBase& getInstance();

		virtual ~NpEngineBase();

		// this should only ever be called when a window is being destroyed
		void removeWindowFromEventQueue(NpWindow& window);

		void onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function) override;

		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;

		void operator()(const char* exp, const char* file, int line, bool& ignore) override;

		void acquireGlContext(NpWindow& window);

		void releaseGlContext(NpWindow& window);

		void pollEvents();

		bool uninitializeGlAsset(PxeGLAsset& asset, bool blocking = false);

		void initializeGlAsset(PxeGLAsset& asset);

		void shutdown() override;

	protected:
		void onDelete() override;

	private:
		static NpEngineBase* s_instance;

		NpEngineBase(PxeLogHandler& logHandler);
		void initPhys();
		void deinitPhys();
		void initSDL();
		void deinitSDL();
		void uninitializeAssets();

		// map of all the windows registered to receive events
		// this will always be all windows that exist
		std::unordered_map<uint32_t, NpWindow*> _eventWindows;
		std::list<PxeGLAsset*> _assetUninitializationQueue;
		std::recursive_mutex _glContextMutex;
		std::mutex _glWaitMutex;
		NpWindow* _activeWindow;
		std::thread::id _boundContextThread;
		PxeLogHandler& _logHandler;
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
		bool _physInit;
		bool _sdlInit;
		bool _engineInit;
	};
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_