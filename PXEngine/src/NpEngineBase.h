#include <mutex>

#include "PxeEngineBase.h"
#include "PxeLogger.h"
#include "PxPhysicsAPI.h"
#include "GL/glew.h"
#include "SDL.h"
#include "NpWindow.h"

#ifndef PXENGINE_NONPUBLIC_ENGINEBASE_H_
#define PXENGINE_NONPUBLIC_ENGINEBASE_H_
namespace pxengine::nonpublic {
	// TODO add NpEngineBase class description (copy EngineBase description)
	class NpEngineBase : public PxeEngineBase, PxeLogHandler, physx::PxAssertHandler, physx::PxErrorCallback
	{
	public:
		// creates and returns an NpEngineBase instance
		// there should only be one instance per process
		// returns nullptr on error or if an NpEngineBase instance already exists
		static NpEngineBase* createInstance(PxeLogHandler& logHandler);

		// returns the NpEngineBase instance
		static NpEngineBase& getInstance();

		virtual ~NpEngineBase();

		PxeWindow* createWindow(uint32_t width, uint32_t height, const char* title) override;

		void onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function) override;

		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;

		void operator()(const char* exp, const char* file, int line, bool& ignore) override;

		void acquireGlContext(NpWindow& window);

		void releaseGlContext(NpWindow& window);

	protected:
		void onDelete() override;

	private:
		static NpEngineBase* s_instance;

		NpEngineBase(PxeLogHandler& logHandler);
		void initPhys();
		void deinitPhys();
		void initSDL();
		void deinitSDL();

		std::mutex _glContextMutex;
		NpWindow* _activeWindow;
		PxeLogHandler& _logHandler;
		physx::PxAssertHandler& _defaultPhysAssertHandler;
		physx::PxFoundation* _physFoundation;
		physx::PxPvd* _physPVD;
		physx::PxPvdTransport* _physPVDTransport;
		physx::PxPhysics* _physPhysics;
		physx::PxCooking* _physCooking;
		physx::PxDefaultAllocator _physAllocator;
		physx::PxTolerancesScale _physScale;
		SDL_GLContext _sdlGlContext;
		bool _physInit;
		bool _sdlInit;
	};
}
#endif // !PXENGINE_NONPUBLIC_NPENGINEBASE_H_
