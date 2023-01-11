#include <iostream>
#include <cassert>
#include <thread>
#include <filesystem>

#include "PxeEngine.h"
#include "PxeWindow.h"
#include "SDL.h"
#include "GL/glew.h"
#include "PxeRenderTexture.h"

#include "TestAsset.h"
#include "LogHandler.h"
#include "TestRenderObject.h"
#include "TestTextureRenderObject.h"

//void runWindow(pxengine::PxeEngineBase* engineBase, uint32_t width, uint32_t height, const char* title, float r, float g, float b) {
//	pxengine::PxeWindow* window = engineBase->createWindow(width, height, title);
//	pxengine::PxeShader* shader = engineBase->loadShader(pxengine::getAssetPath("shaders") / "test.pxeshader");
//	assert(shader);
//	window->acquireGlContext();
//	shader->initializeAsset(true);
//	shader->unbind();
//	shader->bind();
//	shader->unbind();
//	shader->uninitializeAsset(true);
//	window->releaseGlContext();
//	shader->drop();
//	shader = nullptr;
//	TestAsset* t = new TestAsset();
//	t->initializeAsset();
//	pxengine::PxeScene* scene = engineBase->createScene();
//	assert(window);
//	assert(scene);
//	window->setScene(scene);
//	window->setSwapInterval(1);
//	while (!window->getShouldClose())
//	{
//		// 1 should be delta time
//		scene->simulatePhysics(1);
//		window->acquireGlContext();
//		window->setWindowShown(true);
//		//std::cout << title << " Draw" << std::endl;
//		glClearColor(r, g, b, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		window->drawScene();
//		window->swapFrameBuffer();
//		SDL_Event e;
//		while (window->pollEvents(&e));
//		//t->uninitializeAsset(true);
//		window->releaseGlContext();
//	}
//
//	t->drop();
//	scene->drop();
//	window->drop();
//}

int main(int argc, char* args[]) {
	LogHandle h;
	pxengine::PxeEngineBase* engineBase = pxengine::createPXEEngineBase(h);
	pxengine::PxeShader* shader = engineBase->loadShader(pxengine::getAssetPath("shaders") / "test.pxeshader");
	pxengine::PxeWindow* window = engineBase->createWindow(600, 400, "Test Render Window");
	pxengine::PxeScene* scene = engineBase->createScene();

	shader->initializeAsset();
	pxengine::PxeRenderMaterial* material = new pxengine::PxeRenderMaterial(*shader);
	shader->drop();
	shader = nullptr;

	pxengine::PxeRenderTexture* texture = new pxengine::PxeRenderTexture();
	texture->loadImage(pxengine::getAssetPath("textures") / "test.png");
	texture->initializeAsset();
	material->setTexture("u_Texture", *texture, 0);
	texture->drop();
	texture = nullptr;

	material->setProperty4f("u_Color", glm::vec4(1, 1, 1, 1));
	TestTexturedRenderObject* testObj = new TestTexturedRenderObject(*material);
	//TestRenderObject* testObj = new TestRenderObject(*material);
	material->drop();
	material = nullptr;

	for (uint32_t i = 0; i < 1; ++i) {
		scene->addRenderable(static_cast<pxengine::PxeRenderBase&>(*testObj));
	}

	testObj->drop();
	testObj = nullptr;

	window->setScene(scene);
	scene->drop();
	scene = nullptr;

	uint32_t next = SDL_GetTicks() + 1000;
	uint32_t frameCount = 0;
	window->setSwapInterval(0);
	while (!window->getShouldClose())
	{
		++frameCount;
		window->acquireGlContext();
		window->setWindowShown(true);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window->drawScene();
		window->swapFrameBuffer();
		SDL_Event e;
		while (window->pollEvents(&e));
		window->releaseGlContext();
		
		if (SDL_TICKS_PASSED(SDL_GetTicks(), next)) {
			std::cout << "FPS: " << frameCount << std::endl;
			frameCount = 0;
			next = SDL_GetTicks() + 1000;
		}
	}

	window->setScene(nullptr);
	window->acquireGlContext();
	window->releaseGlContext();
	window->drop();
	window = nullptr;

	//std::thread win1(runWindow, engineBase, 500, 500, "Win 1", 1, 0, 0);
	//std::thread win2(runWindow, engineBase, 500, 500, "Win 2", 0, 1, 0);
	//win1.join();
	//win2.join();
	engineBase->shutdown();
	return 0;
}
