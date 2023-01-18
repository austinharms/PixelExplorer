#include "NpScene.h"

#include "NpLogger.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderObject.h"
#include "PxeRenderElement.h"
#include "NpEngine.h"

namespace pxengine::nonpublic {
	NpScene::NpScene(physx::PxScene* scene) : _viewMatrix(glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0), glm::vec3(0, 1, 0)))
	{
		_physScene = scene;
		_physScene->userData = this;
		_simulationAccumulator = 0;
		_simulationTimestep = 0.25f;
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

	const std::list<PxeRenderBase*>& NpScene::getRenderList(PxeRenderPass pass) const
	{
		constexpr size_t renderSize = sizeof(_renderables) / sizeof(*_renderables);
		if ((int8_t)pass < 0 || (int8_t)pass > renderSize) {
			PXE_FATAL("Attempted to get render objects for invalid PxeRenderPass");
		}

		return _renderables[(int8_t)pass];
	}

	void NpScene::simulatePhysics(float time)
	{
		_simulationAccumulator += time;
		while (_simulationAccumulator >= _simulationTimestep)
		{
			_simulationAccumulator -= _simulationTimestep;
			_physScene->simulate(_simulationTimestep);
			_physScene->fetchResults(true);
		}
	}

	float NpScene::getSimulationAccumulator() const
	{
		return _simulationAccumulator;
	}

	void NpScene::setSimulationAccumulator(float time)
	{
		_simulationAccumulator = time;
	}

	float NpScene::getSimulationStep() const
	{
		return _simulationTimestep;
	}

	void NpScene::setSimulationStep(float step)
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
			PXE_WARN("Re-added PxeRenderBase to PxeScene");
		}
#endif // PXE_DEBUG

		renderable.grab();
		if (renderPass == (int8_t)PxeRenderPass::WORLD_SPACE) {
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

	PXE_NODISCARD const glm::mat4& NpScene::getViewMatrix() const
	{
		return _viewMatrix;
	}

	void NpScene::setViewMatrix(const glm::mat4& view)
	{
		_viewMatrix = view;
	}
}