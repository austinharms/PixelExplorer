#include "PxeApplication.h"

#include <new>
#include <thread>
#include <mutex>
#include <condition_variable>
#ifdef PXE_DEBUG
#include <iostream>
#endif // PXE_DEBUG

#include "SDL_timer.h"
#include "PxeScene.h"
#include "PxeWindow.h"
#include "PxeLogger.h"
#include "PxeOSHelpers.h"
#include "PxeRefCount.h"

namespace pxengine {
	struct PxeApplication::Impl
	{
		PxeEngine* _engine;
		std::mutex _workerStartConditionMutex;
		std::mutex _workerStatusConditionMutex;
		std::condition_variable _workerStartConditionVar;
		std::condition_variable _workerStatusConditionVar;
		bool _physicsWorkCompleted;
		bool _objectsWorkCompleted;
		bool _killThreads;
	};

	PxeApplication::PxeApplication()
	{
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeApplication::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeApplication::STORAGE_ALIGN need be changed");

		new(&_storage) Impl();
	}

	PxeApplication::~PxeApplication()
	{
		impl().~Impl();
#ifdef PXE_DEBUG
		PxeSize refCount;
		if ((refCount = PxeRefCount::getPxeRefCountInstanceCount()))
			std::cout << "[WARN] there are still " << refCount << " PxeRefCount instances, this may indicate a memory leak or incorrect reference tracking" << std::endl;
#endif // PXE_DEBUG

	}

	void PxeApplication::start(PxeLogInterface& logInterface)
	{
		Impl& imp = impl();
		imp._killThreads = false;
		imp._physicsWorkCompleted = true;
		imp._objectsWorkCompleted = true;
		imp._engine = new(std::nothrow) PxeEngine(logInterface);
		if (!imp._engine) {
			logInterface.onLog(PxeLogLevel::PXE_FATAL, "Failed to allocate PxeEngine", __FILE__, __FUNCTION__, __LINE__);
			return;
		}

		onStart();
		imp._engine->updateAssetProcessing();
		applicationUpdateLoop();
		onStop();
		imp._engine->shutdownEngine();
		imp._engine = nullptr;
	}

	void PxeApplication::applicationUpdateLoop()
	{
		setThreadName(L"PXE_ApplicationThread");
		Impl& imp = impl();
		std::thread physicsThread(&PxeApplication::physicsUpdateThreadLoop, this);
		std::thread objectThread(&PxeApplication::objectUpdateThreadLoop, this);
		while (!imp._killThreads)
		{
			if (imp._engine->getShutdownPending()) {
				imp._killThreads = true;
				break;
			}

			updateApplication();
		}

		imp._workerStartConditionVar.notify_all();
		physicsThread.join();
		objectThread.join();
	}

	void PxeApplication::physicsUpdateThreadLoop()
	{
		setThreadName(L"PXE_PhysicsThread");
		Impl& imp = impl();
		while (!imp._killThreads)
		{
			std::unique_lock lock(imp._workerStartConditionMutex);
			imp._workerStartConditionVar.wait(lock, [&]() { return !impl()._physicsWorkCompleted || impl()._killThreads; });
			lock.unlock();
			if (imp._killThreads) break;
			updatePhysics();
			imp._workerStatusConditionMutex.lock();
			imp._physicsWorkCompleted = true;
			imp._workerStatusConditionMutex.unlock();
			imp._workerStatusConditionVar.notify_all();
		}
	}

	void PxeApplication::objectUpdateThreadLoop()
	{
		setThreadName(L"PXE_UpdateThread");
		Impl& imp = impl();
		while (!imp._killThreads)
		{
			std::unique_lock lock(imp._workerStartConditionMutex);
			imp._workerStartConditionVar.wait(lock, [&]() { return !impl()._objectsWorkCompleted || impl()._killThreads; });
			lock.unlock();
			if (imp._killThreads) break;
			updateObjects();
			imp._workerStatusConditionMutex.lock();
			imp._objectsWorkCompleted = true;
			imp._workerStatusConditionMutex.unlock();
			imp._workerStatusConditionVar.notify_all();
		}
	}

	void PxeApplication::updateApplication()
	{
		Impl& imp = impl();
		imp._engine->getInputManager().processEvents();
		imp._engine->updateAssetProcessing();
		imp._engine->getRenderPipeline().updateFontAtlas();
		imp._engine->updateDeltaTime();
		imp._workerStartConditionMutex.lock();
		imp._physicsWorkCompleted = false;
		imp._objectsWorkCompleted = false;
		imp._workerStartConditionMutex.unlock();
		imp._workerStartConditionVar.notify_all();
		updateRendering();
		std::unique_lock waitLock(imp._workerStatusConditionMutex);
		imp._workerStatusConditionVar.wait(waitLock, [&]() { return impl()._physicsWorkCompleted && impl()._objectsWorkCompleted; });
		waitLock.unlock();
	}

	void PxeApplication::updateRendering()
	{
		Impl& imp = impl();
		const std::unordered_map<uint32_t, PxeWindow*>& windows = imp._engine->acquireWindows();
		PxeRenderPipeline& renderPipe = imp._engine->getRenderPipeline();
		for (auto pair : windows) {
			pair.second->prepareForRender();
			renderPipe.render(*pair.second);
		}

		// Swap buffers after to minimize gl context switching
		for (auto pair : windows) 
			pair.second->swapFramebuffers();
		imp._engine->bindPrimaryGlContext();
		imp._engine->releaseWindows();
	}

	void PxeApplication::updatePhysics()
	{
		Impl& imp = impl();
		prePhysics();
		const std::list<PxeScene*>& scenes = imp._engine->acquireScenes();
		for (PxeScene* scene : scenes)
			scene->updatePhysics(imp._engine->getDeltaTime());
		imp._engine->releaseScenes();
		postPhysics();
	}

	void PxeApplication::updateObjects()
	{
		Impl& imp = impl();
		onUpdate();
		const std::list<PxeScene*>& scenes = imp._engine->acquireScenes();
		for (PxeScene* scene : scenes)
			scene->updateComponents();
		imp._engine->releaseScenes();
		postUpdate();
	}
}
