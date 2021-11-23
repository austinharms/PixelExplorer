#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <thread>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "reactphysics3d/reactphysics3d.h"
#include "rendering/Renderer.h"
#include "rendering/Shader.h"
#include "rendering/TestRenderable.h"
#include "World.h"
#include "chunk/block/BaseBlock.h"
#include "chunk/Chunk.h"

int main(void) {
	Renderer* renderer = new Renderer(1200, 800, "Pixel Explorer V2.0");
	renderer->setCursorHidden(false);
	World::SetRenderer(renderer);
	World::LoadWorld();
	World::GetChunkManager()->LoadChunk(glm::vec<3, int32_t>(0, 0, 0));
	glm::vec3 camPos(-12, -12, -75);
	glm::vec3 camRot(0);
	float moveSpeed = 5;

	while (renderer->getWindowOpen()) {
		if (renderer->getKeyPressed(GLFW_KEY_W)) {
			camPos.z += renderer->getDeltaTime() * moveSpeed;
		}
		else if (renderer->getKeyPressed(GLFW_KEY_S)) {
			camPos.z -= renderer->getDeltaTime() * moveSpeed;
		}

		if (renderer->getKeyPressed(GLFW_KEY_A)) {
			camPos.x += renderer->getDeltaTime() * moveSpeed;
		}
		else if (renderer->getKeyPressed(GLFW_KEY_D)) {
			camPos.x -= renderer->getDeltaTime() * moveSpeed;
		}

		if (renderer->getKeyPressed(GLFW_KEY_SPACE)) {
			camPos.y -= renderer->getDeltaTime() * moveSpeed;
		}
		else if (renderer->getKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
			camPos.y += renderer->getDeltaTime() * moveSpeed;
		}

		camRot.x += 0.1 * renderer->getDeltaTime();
		if (camRot.x > 360) {
			camRot.x -= 360;
		}

		if (camRot.x < 0) {
			camRot.x += 360;
		}

		renderer->setTransform(camPos, camRot);
		renderer->drawFrame();
	}

	//ORDER IMPORTANT
	World::UnloadWorld();
	World::SetRenderer(nullptr);
	Material::getDefault()->drop();
	Shader::getDefault()->drop();
	renderer->drop();
	//END OF ORDER IMPORTANT
	Chunk::freeEmptyBuffer();
	_CrtDumpMemoryLeaks();
	return 0;
}