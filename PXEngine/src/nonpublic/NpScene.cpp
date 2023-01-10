#include "NpScene.h"

#include "NpLogger.h"
#include "PxeRenderMaterial.h"
#include "PxeRenderObject.h"
#include "PxeRenderElement.h"

namespace pxengine::nonpublic {
	NpScene::NpScene(physx::PxScene* scene)
	{
		_engine = &NpEngineBase::getInstance();
		_engine->grab();
		_physScene = scene;
		_physScene->userData = this;
		_simulationAccumulator = 0;
		_simulationTimestep = 0.25f;
	}

	NpScene::~NpScene()
	{
		_engine->drop();
		_engine = nullptr;
		_physScene->release();
		_physScene = nullptr;
	}

	std::list<PxeRenderBase*>& NpScene::getRenderList()
	{
		return _renderables;
	}

	void NpScene::simulatePhysics(float time, bool blocking)
	{
		_simulationAccumulator += time;
		while (_simulationAccumulator >= _simulationTimestep)
		{
			_simulationAccumulator -= _simulationTimestep;
			_physScene->simulate(_simulationTimestep);
			_physScene->fetchResults(blocking);
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
#ifdef PXE_DEBUG
		if (std::find(_renderables.begin(), _renderables.end(), &renderable) != _renderables.end()) {
			PXE_ERROR("Attempted to add PxeRenderBase already added to PxeScene");
			return;
		}
#endif // PXE_DEBUG

		renderable.grab();
		switch (renderable.getRenderSpace())
		{
		case pxengine::PxeRenderBase::RenderSpace::SCREEN_SPACE:
			_renderables.emplace_back(&renderable);
			break;
		case pxengine::PxeRenderBase::RenderSpace::WORLD_SPACE:
		{
			void* materialPtr = &(static_cast<PxeRenderObject&>(renderable).getRenderMaterial());
			if (_renderables.empty()) {
				_renderables.emplace_front(&renderable);
			}
			else {
				// TODO Optimize this to also sort by shaders in materials to allow for minimal shader changes when rendering
				for (auto it = _renderables.begin(); it != _renderables.end(); ++it) {
					if ((*it)->getRenderSpace() == PxeRenderBase::RenderSpace::SCREEN_SPACE || materialPtr <= (void*)&(static_cast<PxeRenderObject*>(*it)->getRenderMaterial())) {
						_renderables.emplace(it, &renderable);
						break;
					}
				}
			}
		}
		break;
		default:
			PXE_ERROR("Attempted to add PxeRenderBase with invalid RenderSpace to PxeScene");
			renderable.drop();
			break;
		}
	}

	void NpScene::removeRenderable(PxeRenderBase& renderable)
	{
		if (!_renderables.remove(&renderable)) {
			PXE_WARN("Attempted to remove PxeRenderBase not added to PxeScene");
		}
	}
}