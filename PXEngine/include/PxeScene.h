#ifndef PXENGINE_SCENE_H_
#define PXENGINE_SCENE_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeRenderBase.h"

namespace pxengine {
	// Collection of renderable objects and wrapper for physx::PxScene
	class PxeScene : public PxeRefCount
	{
	public:
		PxeScene() = default;
		virtual ~PxeScene() = default;

		// Returns the amount of time in seconds in the physics simulation accumulator
		virtual PXE_NODISCARD float getSimulationAccumulator() const = 0;

		// Sets the amount of time in seconds in the physics simulation accumulator
		virtual void setSimulationAccumulator(float time) = 0;

		// Returns the physics simulation step in seconds
		virtual PXE_NODISCARD float getSimulationStep() const = 0;

		// Sets the physics simulation step in seconds
		// Note: this should only be changed before the first call to simulatePhysics for best simulation stability
		virtual void setSimulationStep(float step) = 0;

		// Add a PxeRenderBase aka a renderable object to the scene
		// Note: things rendered in PxeRenderPass::SCREEN_SPACE aka PxeRenderElement are rendered in the order added
		// things rendered in PxeRenderPass::WORLD_SPACE are ordered by the shader and material used
		virtual void addRenderable(PxeRenderBase& renderable) = 0;

		// Remove a PxeRenderBase aka a renderable object from the scene
		virtual void removeRenderable(PxeRenderBase& renderable) = 0;

		// Returns the view matrix used when the scene is rendered
		virtual PXE_NODISCARD const glm::mat4& getViewMatrix() const = 0;

		// Sets the view matrix used when the scene is rendered
		virtual void setViewMatrix(const glm::mat4& view) = 0;

		// Returns the user data void*
		// Note: this is purely for application uses and is not used by the engine 
		virtual PXE_NODISCARD void* getUserData() const = 0;

		// Sets the user data void*
		// Note: this is purely for application uses and is not used by the engine 
		virtual void setUserData(void* data) = 0;

		PxeScene(const PxeScene& other) = delete;
		PxeScene operator=(const PxeScene& other) = delete;
	};
}
#endif // !PXENGINE_SCENE_H_
