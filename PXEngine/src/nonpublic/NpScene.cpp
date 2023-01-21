#include "NpScene.h"

#include "NpLogger.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderObject.h"
#include "PxeRenderElement.h"
#include "NpEngine.h"

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
		for (int8_t i = 0; i < (int8_t)PxeRenderPass::RENDER_PASS_COUNT; ++i) {
			for (auto it = _renderables[i].begin(); it != _renderables[i].end(); ++it) {
				(*it)->drop();
			}

			_renderables[i].clear();
		}

		_physScene->release();
		NpEngine::getInstance().drop();
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
		_simulationAccumulator += time * _simulationScale;
		if (_simulationAccumulator < _simulationTimestep) return;
		_physScene->lockWrite(__FUNCTION__, __LINE__);
		const std::list<PxeRenderBase*>& physicsRenderables = getRenderList(PxeRenderPass::PHYSICS_WORLD_SPACE);
		for (auto renderable : physicsRenderables)
			static_cast<PxePhysicsRenderObject*>(renderable)->updatePhysicsPosition();

		while (_simulationAccumulator >= _simulationTimestep)
		{
			_simulationAccumulator -= _simulationTimestep;
			_physScene->simulate(_simulationTimestep);
			_physScene->fetchResults(true);
		}

		_physScene->unlockWrite();
	}

	PXE_NODISCARD physx::PxScene* NpScene::getPhysicsScene() const
	{
		return _physScene;
	}

	void NpScene::setPhysicsSimulationSpeed(float speed)
	{
		_simulationScale = speed;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationSpeed() const
	{
		return _simulationScale;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationAccumulator() const
	{
		return _simulationAccumulator;
	}

	void NpScene::setPhysicsSimulationAccumulator(float time)
	{
		_simulationAccumulator = time;
	}

	PXE_NODISCARD float NpScene::getPhysicsSimulationStep() const
	{
		return _simulationTimestep;
	}

	void NpScene::setPhysicsSimulationStep(float step)
	{
		_simulationTimestep = step;
	}

	void NpScene::addRenderable(PxeRenderBase& renderable)
	{
		constexpr size_t renderSize = sizeof(_renderables) / sizeof(*_renderables);
		int8_t renderPass = (int8_t)renderable.getRenderPass();
		if (renderPass < 0 || renderPass > renderSize) {
			PXE_ERROR("Attempted to add PxeRenderBase with invalid PxeRenderPass");
			return;
		}

#ifdef PXE_DEBUG
		if (std::find(_renderables[renderPass].begin(), _renderables[renderPass].end(), &renderable) != _renderables[renderPass].end()) {
			if (renderPass == (int8_t)PxeRenderPass::PHYSICS_WORLD_SPACE) {
				PXE_ERROR("Re-added PxePhysicsRenderObject to PxeScene");
			}
			else {
				PXE_WARN("Re-added PxeRenderBase to PxeScene");
			}
		}
#endif // PXE_DEBUG

		renderable.grab();
		if (renderPass == (int8_t)PxeRenderPass::PHYSICS_WORLD_SPACE) {
			_physScene->addActor(static_cast<PxePhysicsRenderObject&>(renderable).getPhysicsActor());
		}

		if (renderPass == (int8_t)PxeRenderPass::WORLD_SPACE || renderPass == (int8_t)PxeRenderPass::PHYSICS_WORLD_SPACE) {
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
		constexpr size_t renderSize = sizeof(_renderables) / sizeof(*_renderables);
		int8_t renderPass = (int8_t)renderable.getRenderPass();
		if (renderPass < 0 || renderPass > renderSize) {
			PXE_ERROR("Attempted to remove PxeRenderBase with invalid PxeRenderPass");
			return;
		}

		if (!_renderables[renderPass].remove(&renderable)) {
			PXE_WARN("Attempted to remove PxeRenderBase not added to PxeScene");
		}
	}

	PXE_NODISCARD void* NpScene::getUserData() const {
		return _userData;
	}

	void NpScene::setUserData(void* data) {
		_userData = data;
	}
}