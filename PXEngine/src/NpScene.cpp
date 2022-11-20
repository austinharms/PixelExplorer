#include "NpScene.h"

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
}