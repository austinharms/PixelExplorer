#ifndef PXENGINE_NONPUBLIC_SCENE_H_
#define PXENGINE_NONPUBLIC_SCENE_H_
#include <list>

#include "PxeTypes.h"
#include "PxeScene.h"
#include "PxScene.h"
#include "PxeRenderBase.h"

namespace pxengine::nonpublic {
	class NpScene : public PxeScene
	{
	public:
		//############# PxeScene API ##################

		PXE_NODISCARD float getSimulationAccumulator() const override;
		void setSimulationAccumulator(float time) override;
		PXE_NODISCARD float getSimulationStep() const override;
		void setSimulationStep(float step) override;
		void addRenderable(PxeRenderBase& renderable) override;
		void removeRenderable(PxeRenderBase& renderable) override;
		PXE_NODISCARD const glm::mat4& getViewMatrix() const override;
		void setViewMatrix(const glm::mat4& view) override;


		//############# PRIVATE API ##################
	
		NpScene(physx::PxScene* scene);
		virtual ~NpScene();
		PXE_NODISCARD const std::list<PxeRenderBase*>& getRenderList(PxeRenderPass pass) const;
		void simulatePhysics(float time);

	private:
		physx::PxScene* _physScene;
		std::list<PxeRenderBase*> _renderables[(int8_t)PxeRenderPass::RENDER_PASS_COUNT];
		glm::mat4 _viewMatrix;
		float _simulationTimestep;
		float _simulationAccumulator;
	};
}
#endif // !PXENGINE_SCENE_H_
