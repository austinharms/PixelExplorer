#include "PxeScene.h"

#include "PxPhysicsAPI.h"
#include "NpEngineBase.h"

#ifndef PXENGINE_NONPUBLIC_SCENE_H_
#define PXENGINE_NONPUBLIC_SCENE_H_
namespace pxengine::nonpublic {
	class NpScene : public PxeScene
	{
	public:
		//############# PUBLIC API ##################

		// simulates {time} seconds for all the physics bodies in the scene 
		void simulatePhysics(float time, bool blocking) override;

		// returns the amount of time in ms in the physics simulation accumulator
		float getSimulationAccumulator() const override;

		// sets the amount of time in ms in the physics simulation accumulator
		void setSimulationAccumulator(float time) override;

		// returns the physics simulation step in ms
		float getSimulationStep() const override;

		// sets the physics simulation step in ms
		// note: this should only be changed before the first call to simulatePhysics for best simulation stability
		void setSimulationStep(float step) override;

		//############# PRIVATE API ##################
	
		NpScene(physx::PxScene* scene);
		virtual ~NpScene();

	private:
		NpEngineBase* _engine;
		physx::PxScene* _physScene;
		float _simulationTimestep;
		float _simulationAccumulator;
	};
}
#endif // !PXENGINE_SCENE_H_
