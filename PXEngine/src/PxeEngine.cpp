#include "PxeEngine.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

#include "PxeOSHelpers.h"
#include "NpLogger.h"
#include "NpWindow.h"
#include "NpEngine.h"
#include "NpScene.h"
#include "NpInputManager.h"
#include "NpRenderPipeline.h"
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
		void runUpdateThread(std::mutex* startMutex, std::mutex* completeMutex, std::condition_variable* startCondition, std::condition_variable* completeCondition, bool* runFlag, bool* stopFlag, PxeApplicationInterface* application) {
			setThreadName(L"PXE_UpdateThread");
			while (true)
			{
				std::unique_lock lock(*startMutex);
				startCondition->wait(lock, [runFlag, stopFlag]() { return *runFlag || *stopFlag; });
				lock.unlock();
				if (*stopFlag) return;

#ifdef PXE_DEBUG_TIMING
				uint64_t time = SDL_GetTicks64();
#endif
				application->onUpdate();
#ifdef PXE_DEBUG_TIMING
				PXE_INFO("Update thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif

				completeMutex->lock();
				*runFlag = false;
				completeMutex->unlock();
				completeCondition->notify_all();
			}
		}

		void runPhysicsThread(std::mutex* startMutex, std::mutex* completeMutex, std::condition_variable* startCondition, std::condition_variable* completeCondition, bool* runFlag, bool* stopFlag, PxeApplicationInterface* application, nonpublic::NpEngine* engine) {
			setThreadName(L"PXE_PhysicsThread");
			using namespace nonpublic;
			while (true)
			{
				std::unique_lock lock(*startMutex);
				startCondition->wait(lock, [runFlag, stopFlag]() { return *runFlag || *stopFlag; });
				lock.unlock();
				if (*stopFlag) return;

#ifdef PXE_DEBUG_TIMING
				uint64_t time = SDL_GetTicks64();
#endif

				application->prePhysics();
				engine->acquireScenesReadLock();
				const std::list<NpScene*>& scenes = engine->getScenes();
				for (NpScene* scene : scenes) {
					if (scene->getUpdateFlag(PxeSceneUpdateFlags::PHYSICS_UPDATE)) {
						scene->simulatePhysics(engine->getDeltaTime());
					}
				}

				engine->releaseScenesReadLock();
				application->postPhysics();

#ifdef PXE_DEBUG_TIMING
				PXE_INFO("Physics thread took: " + std::to_string(SDL_GetTicks64() - time));
#endif

				completeMutex->lock();
				*runFlag = false;
				completeMutex->unlock();
				completeCondition->notify_all();
			}
		}

		void runRenderThread(PxeApplicationInterface* application, nonpublic::NpEngine* engine)
		{
			using namespace nonpublic;
#ifdef PXE_DEBUG_TIMING
			uint64_t time = SDL_GetTicks64();
#endif
			engine->acquireWindowsReadLock();
			engine->acquireScenesReadLock();
			NpRenderPipeline& renderPipeline = engine->getNpRenderPipeline();
			const std::unordered_map<uint32_t, NpWindow*>& windows = engine->getWindows();
			for (auto it = windows.begin(); it != windows.end(); ++it) {
				NpWindow& window = *(it->second);
				if (window.getAssetStatus() != PxeGLAssetStatus::INITIALIZED || !window.getScene() || !window.getCamera() || !window.getWindowWidth() || !window.getWindowHeight()) continue;
				window.prepareForRender();
				NpScene* scene = window.getNpScene();
				renderPipeline.renderScene(*scene, *window.getRenderTexture(), window.getCamera()->getPVMatrix(), true);
				window.swapFramebuffers();
			}

			engine->releaseScenesReadLock();
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

		std::mutex threadStartMutex;
		std::mutex threadCompleteMutex;
		std::condition_variable threadStartCondition;
		std::condition_variable threadCompleteCondition;
		bool physicsThreadFlag = false;
		bool updateThreadFlag = false;
		bool* updateThreadFlagPtr = &updateThreadFlag;
		bool* physicsThreadFlagPtr = &physicsThreadFlag;
		bool threadStopFlag = false;
		std::thread physicsThread(runPhysicsThread, &threadStartMutex, &threadCompleteMutex, &threadStartCondition, &threadCompleteCondition, &physicsThreadFlag, &threadStopFlag, &application, engine);
		std::thread updateThread(runUpdateThread, &threadStartMutex, &threadCompleteMutex, &threadStartCondition, &threadCompleteCondition, &updateThreadFlag, &threadStopFlag, &application);

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

			engine->getNpRenderPipeline().rebuildGuiFontAtlas();

#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Font processing took: " + std::to_string(SDL_GetTicks64() - tempTime));
#endif

			threadStartMutex.lock();
			physicsThreadFlag = true;
			updateThreadFlag = true;
			threadStartMutex.unlock();
			threadStartCondition.notify_all();
			runRenderThread(&application, engine);

#ifdef PXE_DEBUG_TIMING
			tempTime = SDL_GetTicks64();
#endif

			std::unique_lock lock(threadCompleteMutex);
			threadCompleteCondition.wait(lock, [physicsThreadFlagPtr, updateThreadFlagPtr]() { return !(*physicsThreadFlagPtr || *updateThreadFlagPtr); });
			lock.unlock();

			engine->setDeltaTime((float)(SDL_GetTicks64() - deltaTimer) / 1000.0f);
#ifdef PXE_DEBUG_TIMING
			PXE_INFO("Render thread waited " + std::to_string(SDL_GetTicks64() - tempTime));
			PXE_INFO("Complete frame took " + std::to_string(SDL_GetTicks64() - startTime));
#endif
		}

		threadStartMutex.lock();
		threadStopFlag = true;
		threadStartMutex.unlock();
		threadStartCondition.notify_all();
		updateThread.join();
		physicsThread.join();

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