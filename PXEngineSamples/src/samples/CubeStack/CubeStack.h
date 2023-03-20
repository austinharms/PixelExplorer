#ifndef PXENGINESAMPELS_SCENES_CUBESTACK_H_
#define PXENGINESAMPELS_SCENES_CUBESTACK_H_
#include <cassert>

#include "PxeEngineAPI.h"
#include "PxPhysicsAPI.h"
#include "SampleSceneBase.h"
#include "shared/PxesRenderDataComponent.h"
#include "shared/PxesRenderData.h"
#include "shared/PxesBasicRenderProperties.h"
#include "SDL_keycode.h"
#include "shared/CubeData.h"
#include "PxePhysicsShape.h"
#include "PxeStaticPhysicsActor.h"
#include "PxeDynamicPhysicsActor.h"

class CubeStackSample : public SampleSceneBase
{
public:
	CubeStackSample() {
		using namespace pxengine;
		_window = nullptr;
		_quitAction = nullptr;

		PxeEngine& engine = PxeEngine::getInstance();
		PxeInputManager& inputMgr = engine.getInputManager();

		_window = engine.createWindow(600, 400, "Cube Stack Sample");
		assert(_window);

		_quitAction = inputMgr.getAction("Quit Sample");
		assert(_quitAction);

		if (!_quitAction->hasSource()) {
			PxeActionSource* quitSource = inputMgr.getActionSource(PxeKeyboardActionSourceCode(SDLK_ESCAPE));
			assert(quitSource);
			_quitAction->addSource(*quitSource);
			quitSource->drop();
		}

		pxengine::PxeScene* scene = engine.createScene();
		assert(scene);
		_window->setScene(scene);

		PxeTexture* texture = new PxeTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "cube.png");
		PxesBasicRenderProperties* renderProperties = new PxesBasicRenderProperties(*texture);
		texture->drop();
		PxesRenderData* cubeRenderData = new PxesRenderData();


		physx::PxPhysics& physics = engine.getPhysicsBase();
		physx::PxCooking& cooking = engine.getPhysicsCooking();
		physx::PxConvexMeshDesc meshDesc;
		meshDesc.points.count = 14;
		meshDesc.points.data = cubeRenderVertices;
		meshDesc.points.stride = sizeof(float) * 5;
		meshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eDISABLE_MESH_VALIDATION | physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;
		physx::PxConvexMesh* mesh = cooking.createConvexMesh(meshDesc, physics.getPhysicsInsertionCallback());
		physx::PxMaterial* material = physics.createMaterial(1.0f, 0.5f, 0.5f);
		physx::PxShape* shape = physics.createShape(physx::PxConvexMeshGeometry(mesh), *material);
		material->release();
		mesh->release();

		for (int32_t y = -8; y < 10; ++y) {
			PxeObject* cubeObject = PxeObject::create();
			assert(cubeObject);
			cubeObject->setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0, y * 1.05f, 0)));

			PxesRenderDataComponent* renderComponent = new PxesRenderDataComponent(*cubeRenderData, *renderProperties);
			assert(cubeObject->addComponent(*renderComponent));
			renderComponent->drop();

			PxePhysicsActor* actor;
			if (y == -8) {
				actor = PxeStaticPhysicsActor::create();
			}
			else {
				actor = PxeDynamicPhysicsActor::create();
			}

			assert(actor);
			assert(cubeObject->addComponent(*actor));
			actor->drop();

			PxePhysicsShape* shapeCmp = new PxePhysicsShape(*shape);
			assert(cubeObject->addComponent(*shapeCmp));
			shapeCmp->drop();

			assert(scene->addObject(*cubeObject));
			cubeObject->drop();
		}

		shape->release();
		cubeRenderData->drop();
		renderProperties->drop();
		scene->drop();
	}

	virtual ~CubeStackSample() {
		if (_window) {
			_window->drop();
			_window = nullptr;
		}

		if (_quitAction) {
			_quitAction->drop();
			_quitAction = nullptr;
		}
	}

	bool update() override {
		if (_window->getShouldClose()) return false;
		if (pxengine::PxeEngine::getInstance().getInputManager().getKeyboardFocusedWindow() == _window && _quitAction->getValue()) return false;
		return true;
	}

private:
	pxengine::PxeWindow* _window;
	pxengine::PxeAction* _quitAction;
};
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
