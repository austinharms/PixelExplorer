#include "NpScene.h"

#include "NpLogger.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderObject.h"
#include "PxeRenderElement.h"
#include "NpEngine.h"
#include "PxeDynamicPhysicsRenderObject.h"
#include "PxeStaticPhysicsRenderObject.h"

namespace pxengine::nonpublic {
	NpScene::NpScene(physx::PxScene* scene)
	{
		_physScene = scene;
		_physScene->userData = this;
		_simulationAccumulator = 0;
		_simulationTimestep = 0.01f;
		_simulationScale = 1;
		_userData = nullptr;
		NpEngine::getInstance().grab();
	}

	NpScene::~NpScene()
	{
		_sceneMutex.lock();
		for (int8_t i = 0; i < (int8_t)PxeRenderPass::RENDER_PASS_COUNT; ++i) {
			for (auto it = _renderables[i].begin(); it != _renderables[i].end(); ++it) {
				(*it)->drop();
			}

			_renderables[i].clear();
		}

		_physScene->release();
		NpEngine::getInstance().drop();
		_sceneMutex.unlock();
	}

	PXE_NODISCARD const std::list<PxeRenderBase*>& NpScene::getRenderList(PxeRenderPass pass) const
	{
		constexpr size_t renderSize = sizeof(_renderables) / sizeof(*_renderables);
		if ((int8_t)pass < 0 || (int8_t)pass > renderSize) {
			PXE_FATAL("Attempted to get render objects for invalid PxeRenderPass");
		}

		return _renderables[(int8_t)pass];
	}

	void NpScene::simulatePhysics(float time)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationAccumulator += time * _simulationScale;
		if (_simulationAccumulator < _simulationTimestep) return;
		_physScene->lockWrite(__FUNCTION__, __LINE__);
		_renderableMutex.lock_shared();
		const std::list<PxeRenderBase*>& physicsRenderables = getRenderList(PxeRenderPass::DYNAMIC_PHYSICS_WORLD_SPACE);
		for (auto renderable : physicsRenderables)
			static_cast<PxeDynamicPhysicsRenderObject*>(renderable)->updatePhysicsPosition();
		_renderableMutex.unlock_shared();
		while (_simulationAccumulator >= _simulationTimestep)
		{
			_simulationAccumulator -= _simulationTimestep;
			_physScene->lockWrite();
			_physScene->simulate(_simulationTimestep);
			_physScene->unlockWrite();
			while (!_physScene->checkResults());
			_physScene->lockWrite();
			_physScene->fetchResults(true);
			_physScene->unlockWrite();
		}

		_physScene->unlockWrite();
	}

	PXE_NODISCARD physx::PxScene* NpScene::getPhysicsScene() const
	{
		std::shared_lock lock(_sceneMutex);
		return _physScene;
	}

	void NpScene::setPhysicsSimulationSpeed(float speed)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationScale = speed;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationSpeed() const
	{
		std::shared_lock lock(_sceneMutex);
		return _simulationScale;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationAccumulator() const
	{
		std::shared_lock lock(_sceneMutex);
		return _simulationAccumulator;
	}

	void NpScene::setPhysicsSimulationAccumulator(float time)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationAccumulator = time;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationStep() const
	{
		std::shared_lock lock(_sceneMutex);
		return _simulationTimestep;
	}

	void NpScene::setPhysicsSimulationStep(float step)
	{
		std::unique_lock lock(_sceneMutex);
		_simulationTimestep = step;
	}

	void NpScene::addRenderable(PxeRenderBase& renderable)
	{
		std::unique_lock lock(_renderableMutex);
		constexpr size_t renderSize = sizeof(_renderables) / sizeof(*_renderables);
		int8_t renderPass = (int8_t)renderable.getRenderPass();
		if (renderPass < 0 || renderPass > renderSize) {
			PXE_ERROR("Attempted to add PxeRenderBase with invalid PxeRenderPass");
			return;
		}

#ifdef PXE_DEBUG
		if (std::find(_renderables[renderPass].begin(), _renderables[renderPass].end(), &renderable) != _renderables[renderPass].end()) {
			if (renderPass == (int8_t)PxeRenderPass::DYNAMIC_PHYSICS_WORLD_SPACE || renderPass == (int8_t)PxeRenderPass::STATIC_PHYSICS_WORLD_SPACE) {
				PXE_ERROR("Re-added PxePhysicsRenderObject to PxeScene");
			}
			else {
				PXE_WARN("Re-added PxeRenderBase to PxeScene");
			}
		}
#endif // PXE_DEBUG

		renderable.grab();
		if (renderPass == (int8_t)PxeRenderPass::DYNAMIC_PHYSICS_WORLD_SPACE) {
			PxeDynamicPhysicsRenderObject& physObj = static_cast<PxeDynamicPhysicsRenderObject&>(renderable);
			if (!physObj.getPhysicsActor()) {
				PXE_WARN("Added PxeDynamicPhysicsRenderObject without PxPhysicsActor");
			}
			else {
				_physScene->addActor(*physObj.getPhysicsActor());
			}
		}
		else if (renderPass == (int8_t)PxeRenderPass::STATIC_PHYSICS_WORLD_SPACE) {
			PxeStaticPhysicsRenderObject& physObj = static_cast<PxeStaticPhysicsRenderObject&>(renderable);
			if (!physObj.getPhysicsActor()) {
				PXE_WARN("Added PxeStaticPhysicsRenderObject without PxActor");
			}
			else {
				_physScene->addActor(*physObj.getPhysicsActor());
			}
		}

		// TODO Optimize this to put STATIC_PHYSICS_WORLD_SPACE into WORLD_SPACE queue to allow for minimal shader changes when rendering
		if (renderPass == (int8_t)PxeRenderPass::WORLD_SPACE || renderPass == (int8_t)PxeRenderPass::DYNAMIC_PHYSICS_WORLD_SPACE || renderPass == (int8_t)PxeRenderPass::STATIC_PHYSICS_WORLD_SPACE) {
			void* materialPtr = &(static_cast<PxeRenderObject&>(renderable).getRenderMaterial());
			if (_renderables[renderPass].empty()) {
				_renderables[renderPass].emplace_front(&renderable);
			}
			else {
				// TODO Optimize this to also sort by shaders in materials to allow for minimal shader changes when rendering
				for (auto it = _renderables[renderPass].begin(); it != _renderables[renderPass].end(); ++it) {
					if (materialPtr <= (void*)&(static_cast<PxeRenderObject*>(*it)->getRenderMaterial())) {
						_renderables[renderPass].emplace(it, &renderable);
						break;
					}
				}
			}
		}
		else {
			_renderables[renderPass].emplace_back(&renderable);
		}
	}

	void NpScene::removeRenderable(PxeRenderBase& renderable)
	{
		std::unique_lock lock(_renderableMutex);
		constexpr size_t renderSize = sizeof(_renderables) / sizeof(*_renderables);
		int8_t renderPass = (int8_t)renderable.getRenderPass();
		if (renderPass < 0 || renderPass > renderSize) {
			PXE_ERROR("Attempted to remove PxeRenderBase with invalid PxeRenderPass");
			return;
		}

		if (_renderables[renderPass].remove(&renderable)) {
			renderable.drop();
		}
		else {
			PXE_WARN("Attempted to remove PxeRenderBase not added to PxeScene");
		}
	}

	PXE_NODISCARD void* NpScene::getUserData() const {
		std::shared_lock lock(_sceneMutex);
		return _userData;
	}

	void NpScene::setUserData(void* data) {
		std::unique_lock lock(_sceneMutex);
		_userData = data;
	}

	void NpScene::acquireReadLock()
	{
		_sceneMutex.lock_shared();
	}

	void NpScene::releaseReadLock()
	{
		_sceneMutex.unlock_shared();
	}

	void NpScene::acquireWriteLock()
	{
		_sceneMutex.lock();
	}

	void NpScene::releaseWriteLock()
	{
		_sceneMutex.unlock();
	}

	void NpScene::acquireRenderableReadLock()
	{
		_renderableMutex.lock_shared();
	}

	void NpScene::releaseRenderableReadLock()
	{
		_renderableMutex.unlock_shared();
	}

	void NpScene::acquireRenderableWriteLock()
	{
		_renderableMutex.lock();
	}

	void NpScene::releaseRenderableWriteLock()
	{
		_renderableMutex.unlock();
	}
}