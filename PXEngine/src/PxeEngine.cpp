#include "PxeEngine.h"

#include <thread>
#include <unordered_map>

#include "NpLogger.h"
#include "NpWindow.h"
#include "NpEngine.h"

//#define PXE_DEBUG_TIMING
#ifdef PXE_DEBUG_TIMING
#include <string>
#include "SDL_timer.h"
#endif // PXE_DEBUG_TIMING


namespace pxengine {
	PXE_NODISCARD PxeEngine& pxeGetEngine() {
		return static_cast<PxeEngine&>(nonpublic::NpEngine::getInstance());
	}

	namespace {
		void runUpdateThread(PxeApplicationInterface* application) {
#ifdef PXE_DEBUG_TIMING
			uint64_t time = SDL_GetTicks64();
#endif
			application->onUpdate();
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Update thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif
		}

		void runPhysicsThread(PxeApplicationInterface* application, nonpublic::NpEngine* engine) {
			using namespace nonpublic;
#ifdef PXE_DEBUG_TIMING
			uint64_t time = SDL_GetTicks64();
#endif

			application->prePhysics();
			const std::unordered_map<PxeWindowId, NpWindow*>& windows = engine->getWindows();
			for (auto it = windows.begin(); it != windows.end(); ++it) {
				if (it->second->getScene()) {
					it->second->getNpScene()->simulatePhysics(1.0f);
				}
			}

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

			const std::unordered_map<PxeWindowId, NpWindow*>& windows = engine->getWindows();
			for (auto it = windows.begin(); it != windows.end(); ++it) {
				NpWindow& window = *(it->second);
				if (window.getAssetStatus() != PxeGLAssetStatus::INITIALIZED || !window.getScene()) continue;
				engine->newFrame(window);
				application->preRender(window);
				engine->renderFrame(window);
				application->preGUI(window);
				engine->renderGui(window);
				application->postGUI(window);
				application->postRender(window);
				engine->swapFramebuffer(window);
			}

			engine->processEvents();
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Render thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif
		}
	}
	
	void pxeRunApplication(PxeApplicationInterface& application, PxeLogInterface& logInterface) {
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
#ifdef PXE_DEBUG_TIMING
			startTime = SDL_GetTicks64();
			tempTime = SDL_GetTicks64();
#endif

			engine->processAssetQueue();

#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Asset processing took: " + std::to_string(SDL_GetTicks64() - tempTime));
#endif

			std::thread physicsThread(runPhysicsThread, &application, engine);
			std::thread updateThread(runUpdateThread, &application);
			runRenderThread(&application, engine);

#ifdef PXE_DEBUG_TIMING
			tempTime = SDL_GetTicks64();
#endif

			updateThread.join();
			physicsThread.join();

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