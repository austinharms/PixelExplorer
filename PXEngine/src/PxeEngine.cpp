#include "PxeEngine.h"

#include <thread>
#include <unordered_map>

#include "PxeOSHelpers.h"
#include "NpLogger.h"
#include "NpWindow.h"
#include "NpEngine.h"
#include "NpInputManager.h"
#include "NpFontManager.h"
#include "SDL_timer.h"

//#define PXE_DEBUG_TIMING
#ifdef PXE_DEBUG_TIMING
#include <string>
#endif // PXE_DEBUG_TIMING


namespace pxengine {
	PXE_NODISCARD PxeEngine& pxeGetEngine() {
		return static_cast<PxeEngine&>(nonpublic::NpEngine::getInstance());
	}

	namespace {
		void runUpdateThread(PxeApplicationInterface* application) {
			setThreadName(L"PXE_UpdateThread");
#ifdef PXE_DEBUG_TIMING
			uint64_t time = SDL_GetTicks64();
#endif
			application->onUpdate();
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Update thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif
		}

		void runPhysicsThread(PxeApplicationInterface* application, nonpublic::NpEngine* engine) {
			setThreadName(L"PXE_PhysicsThread");
			using namespace nonpublic;
#ifdef PXE_DEBUG_TIMING
			uint64_t time = SDL_GetTicks64();
#endif
			application->prePhysics();
			engine->acquireWindowsReadLock();
			const std::unordered_map<uint32_t, NpWindow*>& windows = engine->getWindows();
			for (auto it = windows.begin(); it != windows.end(); ++it) {
				it->second->acquireReadLock();
				NpScene* scene = it->second->getNpScene();
				if (scene)
					scene->grab();
				it->second->releaseReadLock();
				if (scene) {
					scene->simulatePhysics(engine->getDeltaTime());
					scene->drop();
				}
			}

			engine->releaseWindowsReadLock();
			application->postPhysics();
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Physics thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif
		}

		void runRenderThread(PxeApplicationInterface* application, nonpublic::NpEngine* engine)
		{
			using namespace nonpublic;
#ifdef PXE_DEBUG_TIMING
			uint64_t time = SDL_GetTicks64();
#endif
			engine->acquireWindowsReadLock();
			const std::unordered_map<uint32_t, NpWindow*>& windows = engine->getWindows();
			for (auto it = windows.begin(); it != windows.end(); ++it) {
				NpWindow& window = *(it->second);
				if (window.getAssetStatus() != PxeGLAssetStatus::INITIALIZED || !window.getScene() || !window.getCamera() || !window.getWindowWidth() || !window.getWindowHeight()) continue;
				engine->newFrame(window);
				application->preRender(window);
				NpScene* scene = window.getNpScene();
				engine->renderFrame(window);
				application->preGUI(window);
				engine->renderGui(window);
				application->postGUI(window);
				application->postRender(window);
				engine->swapFramebuffer(window);
				engine->bindPrimaryGlContext();
			}

			engine->releaseWindowsReadLock();
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Render thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif
		}
	}

	void pxeRunApplication(PxeApplicationInterface& application, PxeLogInterface& logInterface) {
		setThreadName(L"PXE_MainThread");
		using namespace nonpublic;
#ifdef PXE_DEBUG_TIMING
		uint64_t startTime = SDL_GetTicks64();
		uint64_t tempTime = SDL_GetTicks64();
#endif

		NpEngine* engine = new(std::nothrow) NpEngine(logInterface);
		PXE_ASSERT(engine, "Failed to allocate PxeEngine");

#ifdef PXE_DEBUG_TIMING
		PXE_INFO("Engine instantiation took " + std::to_string(SDL_GetTicks64() - tempTime));
		tempTime = SDL_GetTicks64();
#endif

		application.onStart();

#ifdef PXE_DEBUG_TIMING
		PXE_INFO("Application start took " + std::to_string(SDL_GetTicks64() - tempTime));
		tempTime = SDL_GetTicks64();
#endif

		engine->processAssetQueue();

#ifdef PXE_DEBUG_TIMING
		PXE_INFO("Initial asset processing took " + std::to_string(SDL_GetTicks64() - tempTime));
		PXE_INFO("Complete startup took " + std::to_string(SDL_GetTicks64() - startTime));
#endif

		while (!engine->getShutdownFlag())
		{
			uint64_t deltaTimer = SDL_GetTicks64();
#ifdef PXE_DEBUG_TIMING
			startTime = SDL_GetTicks64();
			tempTime = SDL_GetTicks64();
#endif
			engine->bindPrimaryGlContext();
			engine->processAssetQueue();

#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Asset processing took: " + std::to_string(SDL_GetTicks64() - tempTime));
			tempTime = SDL_GetTicks64();
#endif

			engine->getNpInputManager().processEvents();

#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Event processing took: " + std::to_string(SDL_GetTicks64() - tempTime));
			tempTime = SDL_GetTicks64();
#endif

			engine->getNpFontManager().updateFontAtlas();

#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Font processing took: " + std::to_string(SDL_GetTicks64() - tempTime));
#endif

			std::thread physicsThread(runPhysicsThread, &application, engine);
			std::thread updateThread(runUpdateThread, &application);
			runRenderThread(&application, engine);

#ifdef PXE_DEBUG_TIMING
			tempTime = SDL_GetTicks64();
#endif

			updateThread.join();
			physicsThread.join();
			engine->setDeltaTime((float)(SDL_GetTicks64() - deltaTimer) / 1000.0f);
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Render thread waited " + std::to_string(SDL_GetTicks64() - tempTime));
			PXE_INFO("Complete frame took " + std::to_string(SDL_GetTicks64() - startTime));
#endif
		}

#ifdef PXE_DEBUG_TIMING
		tempTime = SDL_GetTicks64();
#endif
		application.onStop();

#ifdef PXE_DEBUG_TIMING
		PXE_INFO("Application stop took " + std::to_string(SDL_GetTicks64() - tempTime));
#endif

		engine->shutdownEngine();
		engine = nullptr;
	}
}