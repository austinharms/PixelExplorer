#ifndef PXENGINESAMPELS_SCENES_MARCHING_CUBES_H_
#define PXENGINESAMPELS_SCENES_MARCHING_CUBES_H_
#include <cstdlib>
#include <ctime>

#include "PxeEngineAPI.h"
#include "../../SampleSceneBase.h"
#include "SDL_keycode.h"
#include "MarchingMesh.h"

class MarchingCubes : public SampleSceneBase
{
public:
	MarchingCubes() {
		using namespace pxengine;
		using namespace marchingcubes;
		_error = false;
		_window = nullptr;
		_quitAction = nullptr;
		_mesh = nullptr;

		PxeEngine& engine = pxeGetEngine();
		PxeInputManager& inputMgr = engine.getInputManager();

		_window = engine.createWindow(600, 400, "Marching Cubes Sample");
		if (!_window) {
			_error = true;
			return;
		}

		_quitAction = inputMgr.getAction("Quit Sample");
		if (!_quitAction) {
			_error = true;
			return;
		}

		_quitAction->grab();
		if (!_quitAction->hasSource()) {
			PxeActionSource* quitSource = inputMgr.getActionSource((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32 | SDLK_ESCAPE);
			if (!quitSource) {
				_error = true;
				return;
			}

			_quitAction->addSource(*quitSource);
		}

		_addAction = inputMgr.getAction("Add Point");
		if (!_addAction) {
			_error = true;
			return;
		}

		_addAction->grab();
		if (!_addAction->hasSource()) {
			PxeActionSource* addSource = inputMgr.getActionSource((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32 | SDLK_a);
			if (!addSource) {
				_error = true;
				return;
			}

			_addAction->addSource(*addSource);
		}

		pxengine::PxeScene* scene = pxeGetEngine().createScene();
		if (!scene) {
			_error = true;
			return;
		}

		_window->setScene(scene);

		PxeShader* shader = engine.getRenderPipeline().loadShader(getAssetPath("shaders") / "test.pxeshader");
		if (!shader) {
			scene->drop();
			_error = true;
			return;
		}

		PxeTexture* texture = new(std::nothrow) PxeTexture();
		if (!texture) {
			_error = true;
			shader->drop();
			scene->drop();
			return;
		}

		texture->loadImage(pxengine::getAssetPath("textures") / "test.png");

		PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader, PxeRenderPass::FORWARD);
		if (!material) {
			_error = true;
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		material->setTexture("u_Texture", *texture, 0);
		material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));

		_mesh = new(std::nothrow) MarchingMesh(*material, 10);
		if (!_mesh) {
			_error = true;
			material->drop();
			texture->drop();
			shader->drop();
			scene->drop();
			return;
		}

		scene->addObject(*_mesh);
		material->drop();
		texture->drop();
		shader->drop();
		scene->drop();
	}

	bool update() override {
		srand(SDL_GetTicks64());
		if (_error || _window->getShouldClose()) return false;
		if (pxengine::pxeGetEngine().getInputManager().getKeyboardFocusedWindow() == _window) {
			if (_quitAction->getValue()) return false;
			if (_addAction->getValue()) {
				glm::u16vec3 pointPos(
					static_cast<uint16_t>(((float)rand() / (float)RAND_MAX) * (_mesh->getGridSize() - 1)),
					static_cast<uint16_t>(((float)rand() / (float)RAND_MAX) * (_mesh->getGridSize() - 1)),
					static_cast<uint16_t>(((float)rand() / (float)RAND_MAX) * (_mesh->getGridSize() - 1))
				);
				_mesh->getPoint(pointPos) = 1;
				_mesh->recalculateMesh();
			}
		}

		return true;
	}

	virtual ~MarchingCubes() {
		if (_window) {
			_window->drop();
			_window = nullptr;
		}

		if (_quitAction) {
			_quitAction->drop();
			_quitAction = nullptr;
		}

		if (_addAction) {
			_addAction->drop();
			_addAction = nullptr;
		}

		if (_mesh) {
			_mesh->drop();
			_mesh = nullptr;
		}
	}

private:
	pxengine::PxeWindow* _window;
	pxengine::PxeAction* _quitAction;
	pxengine::PxeAction* _addAction;
	marchingcubes::MarchingMesh* _mesh;
	bool _error;
};
#endif // !PXENGINESAMPELS_SCENES_CUBEWALL_H_
