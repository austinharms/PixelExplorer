#ifndef PXENGINESAMPELS_SCENES_CUBESTACK_H_
#define PXENGINESAMPELS_SCENES_CUBESTACK_H_
#include <cassert>
#include <vector>

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
#include "StackGui.h"

class CubeStackSample : public SampleSceneBase
{
public:
	CubeStackSample() {
		using namespace pxengine;
		_window = nullptr;
		_quitAction = nullptr;
		_startPoint = -8.0f;
		_cubeSpacing = 1.5f;
		_timeScale = 1.0f;
		_physicsStep = 0.035f;

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
		scene->setPhysicsTimeScale(_timeScale);
		_physicsStep = scene->getPhysicsStepSize();
		_window->setScene(scene);

		PxeTexture* texture = new PxeTexture();
		texture->loadImage(pxengine::getAssetPath("textures") / "cube.png");
		_cubeRenderProperties = new PxesBasicRenderProperties(*texture);
		texture->drop();
		_cubeRenderData = new PxesRenderData();

		physx::PxPhysics& physics = engine.getPhysicsBase();
		physx::PxCooking& cooking = engine.getPhysicsCooking();
		physx::PxConvexMeshDesc meshDesc;
		meshDesc.points.count = cubeRenderVertexCount;
		meshDesc.points.data = cubeRenderVertices;
		meshDesc.points.stride = sizeof(float) * cubeRenderVertexStride;
		meshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eDISABLE_MESH_VALIDATION | physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;
		physx::PxConvexMesh* mesh = cooking.createConvexMesh(meshDesc, physics.getPhysicsInsertionCallback());
		physx::PxMaterial* material = physics.createMaterial(1.0f, 0.5f, 0.5f);
		_cubeShape = physics.createShape(physx::PxConvexMeshGeometry(mesh), *material);
		assert(_cubeShape);
		material->release();
		mesh->release();

		_gui = new StackGui();
		StackGui::State& guiState = _gui->getState();
		guiState.BaseHeight = _startPoint;
		guiState.CubeSpacing = _cubeSpacing;
		guiState.TimeScale = _timeScale;
		guiState.PhysicsStep = _physicsStep;
		PxeObject* cubeObject = PxeObject::create();
		assert(cubeObject);
		_cubes.emplace_back(cubeObject);
		cubeObject->setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0, _startPoint + ((float)(_cubes.size() - 1) * _cubeSpacing), 0)));
		PxesRenderDataComponent* renderComponent = new PxesRenderDataComponent(*_cubeRenderData, *_cubeRenderProperties);
		assert(cubeObject->addComponent(*renderComponent));
		assert(cubeObject->addComponent(*_gui));
		renderComponent->drop();

		PxePhysicsActor* actor = PxeStaticPhysicsActor::create();
		assert(actor);
		assert(cubeObject->addComponent(*actor));
		actor->drop();
		PxePhysicsShape* shapeCmp = new PxePhysicsShape(*_cubeShape);
		assert(cubeObject->addComponent(*shapeCmp));
		shapeCmp->drop();
		assert(_window->getScene()->addObject(*cubeObject));

		for (int32_t i = 1; i < guiState.CubeCount; ++i) {
			addCube();
		}

		scene->drop();
	}

	virtual ~CubeStackSample() {
		for (int32_t i = 0; i < _cubes.size(); ++i)
			_cubes[i]->drop();

		_cubeShape->release();
		_cubeRenderData->drop();
		_cubeRenderProperties->drop();
		_gui->drop();

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
		StackGui::State& guiState = _gui->getState();
		_startPoint = guiState.BaseHeight;
		_cubeSpacing = guiState.CubeSpacing;
		while (guiState.CubeCount != _cubes.size())
		{
			if (guiState.CubeCount > _cubes.size()) {
				addCube();
			}
			else {
				removeCube();
			}
		}

		if (guiState.PhysicsStep != _physicsStep) {
			_physicsStep = guiState.PhysicsStep;
			_window->getScene()->setPhysicsStepSize(_physicsStep);
		}

		if (guiState.TimeScale != _timeScale) {
			_timeScale = guiState.TimeScale;
			_window->getScene()->setPhysicsTimeScale(_timeScale);
		}

		if (guiState.Reset) {
			guiState.Reset = false;
			reset();
		}

		return true;
	}

	void reset() {
		for (int32_t i = 0; i < _cubes.size(); ++i) {
			_cubes[i]->setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0, _startPoint + ((float)i * _cubeSpacing), 0)));
			if (i) {
				physx::PxRigidDynamic& actor = _cubes[i]->getComponent<pxengine::PxeDynamicPhysicsActor>()->getDynamicActor();
				actor.getScene()->lockWrite(__FILE__, __LINE__);
				actor.clearForce(physx::PxForceMode::eACCELERATION);
				actor.clearForce(physx::PxForceMode::eVELOCITY_CHANGE);
				actor.setAngularVelocity(physx::PxVec3(0, 0, 0));
				actor.setLinearVelocity(physx::PxVec3(0, 0, 0));
				actor.getScene()->unlockWrite();
			}		
		}
	}

	void addCube() {
		using namespace pxengine;
		PxeObject* cubeObject = PxeObject::create();
		assert(cubeObject);
		_cubes.emplace_back(cubeObject);
		cubeObject->setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0, _startPoint + ((float)(_cubes.size() - 1) * _cubeSpacing), 0)));
		PxesRenderDataComponent* renderComponent = new PxesRenderDataComponent(*_cubeRenderData, *_cubeRenderProperties);
		assert(cubeObject->addComponent(*renderComponent));
		renderComponent->drop();

		PxePhysicsActor* actor = PxeDynamicPhysicsActor::create();
		assert(actor);
		assert(cubeObject->addComponent(*actor));
		actor->drop();
		PxePhysicsShape* shapeCmp = new PxePhysicsShape(*_cubeShape);
		assert(cubeObject->addComponent(*shapeCmp));
		shapeCmp->drop();
		assert(_window->getScene()->addObject(*cubeObject));
	}

	void removeCube() {
		if (_cubes.size() == 1) return;
		pxengine::PxeObject* obj = _cubes[_cubes.size() - 1];
		_window->getScene()->removeObject(*obj);
		_cubes.pop_back();
		obj->drop();
	}

private:
	pxengine::PxeWindow* _window;
	pxengine::PxeAction* _quitAction;
	StackGui* _gui;
	PxesBasicRenderProperties* _cubeRenderProperties;
	PxesRenderData* _cubeRenderData;
	physx::PxShape* _cubeShape;
	std::vector<pxengine::PxeObject*> _cubes;
	float _startPoint;
	float _cubeSpacing;
	float _timeScale;
	float _physicsStep;
};
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
