#include "PxeRefCount.h"
#include "PxeRenderBase.h"

#ifndef PXENGINE_SCENE_H_
#define PXENGINE_SCENE_H_
namespace pxengine {
	class PxeScene : public PxeRefCount
	{
	public:
		PxeScene() = default;
		virtual ~PxeScene() = default;

		// simulates {time} seconds for all the physics bodies in the scene
		// this call will block thread execution until the simulation step is complete
		// unless blocking is set to false
		virtual void simulatePhysics(float time, bool blocking = true) = 0;

		// returns the amount of time in seconds in the physics simulation accumulator
		virtual float getSimulationAccumulator() const = 0;

		// sets the amount of time in seconds in the physics simulation accumulator
		virtual void setSimulationAccumulator(float time) = 0;

		// returns the physics simulation step in seconds
		virtual float getSimulationStep() const = 0;

		// sets the physics simulation step in seconds
		// note: this should only be changed before the first call to simulatePhysics for best simulation stability
		virtual void setSimulationStep(float step) = 0;

		// add a PxeRenderBase aka a renderable object to the scene
		// note things rendered in RenderSpace::SCREEN_SPACE aka PxeRenderElement are rendered in the order inserted
		// things rendered in RenderSpace::SCREEN_SPACE are rendered by the shader and material used
		virtual void addRenderable(PxeRenderBase& renderable) = 0;

		// remove a PxeRenderBase aka a renderable object from the scene
		virtual void removeRenderable(PxeRenderBase& renderable) = 0;

		PxeScene(const PxeScene& other) = delete;
		PxeScene operator=(const PxeScene& other) = delete;
	};
}
#endif // !PXENGINE_SCENE_H_
