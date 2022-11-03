#include "NpEngineBase.h"

#include <new>

#include "NpLogger.h"

#define PXE_ENGINEBASE_INFO(msg) onLog(msg, (uint8_t)pxengine::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_WARN(msg) onLog(msg, (uint8_t)pxengine::LogLevel::WARN, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_ERROR(msg) onLog(msg, (uint8_t)pxengine::LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_FATAL(msg) onLog(msg, (uint8_t)pxengine::LogLevel::FATAL, __FILE__, __LINE__, __FUNCTION__)
#define PXE_ENGINEBASE_CHECKSDLERROR() do { const char* error = SDL_GetError(); if (*error != '\0') { PXE_ENGINEBASE_ERROR(error); SDL_ClearError(); } } while (0)

namespace pxengine::nonpublic {
	NpEngineBase* NpEngineBase::s_instance = nullptr;

	NpEngineBase* NpEngineBase::createInstance(PxeLogHandler& logHandler)
	{
		if (s_instance) {
			PXE_WARN("PxeEngineBase Already Exist");
			return nullptr;
		}

		s_instance = new(std::nothrow) NpEngineBase(logHandler);
		PXE_ASSERT(s_instance, "Failed to allocate PxeEngineBase");
		PXE_INFO("PxeEngineBase Created");
		return s_instance;
	}

	NpEngineBase& NpEngineBase::getInstance()
	{
		PXE_ASSERT(s_instance, "PxeEngineBase Not Created");
		return *s_instance;
	}

	NpEngineBase::NpEngineBase(PxeLogHandler& logHandler) : _logHandler(logHandler), _defaultPhysAssertHandler(physx::PxGetAssertHandler())
	{
		_physInit = false;
		_sdlInit = false;
		_sdlGlContext = nullptr;
		_activeWindow = nullptr;
		initSDL();
		initPhys();
	}

	void NpEngineBase::initPhys()
	{
		if (_physInit) {
			PXE_ENGINEBASE_WARN("Attempted to initialize already initialized Physics");
			return;
		}

		physx::PxSetAssertHandler(*this);
		_physScale.length = 1;
		_physScale.speed = 9.81;
		_physFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _physAllocator, *this);
		_physPVD = PxCreatePvd(*_physFoundation);
		_physPVDTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 1000);
		_physPVD->connect(*_physPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
		_physPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_physFoundation, _physScale, true, _physPVD);
		physx::PxCookingParams params(_physScale);
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		_physCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_physFoundation, params);
		PxInitExtensions(*_physPhysics, _physPVD);
		_physInit = true;
	}

	void NpEngineBase::deinitPhys()
	{
		if (!_physInit) {
			PXE_ENGINEBASE_WARN("Attempted to deinitialize uninitialized Physics");
			return;
		}

		PxCloseExtensions();
		_physCooking->release();
		_physPhysics->release();
		_physPVD->disconnect();
		_physPVD->release();
		_physPVDTransport->release();
		_physFoundation->release();
		physx::PxSetAssertHandler(_defaultPhysAssertHandler);
		_physInit = false;
	}

	void NpEngineBase::initSDL()
	{
		if (_sdlInit) {
			PXE_ENGINEBASE_WARN("Attempted to initialize already initialized SDL");
			return;
		}

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			PXE_ENGINEBASE_FATAL("Failed to Init SDL");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		// create a throwaway window used to create an OpenGl context
		// TODO find a better way to do this
		SDL_Window* tempWindow = SDL_CreateWindow("PXE_ENGINEBASE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS );
		if (!tempWindow)
			PXE_ENGINEBASE_FATAL("Failed to Create SDL Window");
		PXE_ENGINEBASE_CHECKSDLERROR();

		_sdlGlContext = SDL_GL_CreateContext(_sdlWindow);
		PXE_ENGINEBASE_CHECKSDLERROR();

		SDL_GL_MakeCurrent(_sdlWindow, _sdlGlContext);
		PXE_ENGINEBASE_CHECKSDLERROR();

		if (glewInit() != GLEW_OK)
			PXE_ENGINEBASE_FATAL("Failed to Init GLEW");
		SDL_DestroyWindow(tempWindow);
		PXE_ENGINEBASE_CHECKSDLERROR();
		_sdlInit = true;
	}

	void NpEngineBase::deinitSDL()
	{
		if (!_sdlInit) {
			PXE_ENGINEBASE_WARN("Attempted to deinitialize uninitialized SDL");
			return;
		}

		SDL_GL_DeleteContext(_sdlGlContext);
		_sdlGlContext = nullptr;
		PXE_ENGINEBASE_CHECKSDLERROR();
		SDL_Quit();
		_sdlInit = false;
	}

	NpEngineBase::~NpEngineBase()
	{
		deinitSDL();
		deinitPhys();
		s_instance = nullptr;
		PXE_ENGINEBASE_INFO("PxeEngineBase Destroyed");
	}

	void NpEngineBase::onLog(const char* msg, uint8_t level, const char* file, uint64_t line, const char* function)
	{
		_logHandler.onLog(msg, level, file, line, function);
	}

	void NpEngineBase::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		onLog(message, code, file, line, "UNKNOWN");
	}

	void NpEngineBase::operator()(const char* exp, const char* file, int line, bool& ignore)
	{
		getPXEAssertHandler().onAssert(exp, file, line, "UNKNOWN");
	}

	void NpEngineBase::acquireGlContext(NpWindow& window)
	{
		_glContextMutex.lock();
		_activeWindow = &window;
		SDL_GL_MakeCurrent(&(window.getSDLWindow()), _sdlGlContext);
		PXE_ENGINEBASE_CHECKSDLERROR();
		SDL_GL_SetSwapInterval(window.getSwapInterval());
		PXE_ENGINEBASE_CHECKSDLERROR();
	}

	void NpEngineBase::releaseGlContext(NpWindow& window)
	{
		if ((&window) != _activeWindow) {
			PXE_ENGINEBASE_ERROR("Attempted to release GlContext not acquired by window");
			return;
		}

		_activeWindow = nullptr;
		_glContextMutex.unlock();
	}

	void NpEngineBase::onDelete()
	{
	}
}