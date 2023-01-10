#include <iostream>
#include <cassert>
#include <thread>
#include <filesystem>

#include "PxeEngine.h"
#include "PxeWindow.h"
#include "SDL.h"
#include "GL/glew.h"

#include "TestAsset.h"
#include "LogHandler.h"
#include "TestRenderObject.h"

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
	shader->initializeAsset();
	pxengine::PxeWindow* window = engineBase->createWindow(600, 400, "Test Render Window");
	pxengine::PxeScene* scene = engineBase->createScene();

	pxengine::PxeRenderMaterial* material = new pxengine::PxeRenderMaterial(*shader);
	material->setProperty3f("u_color", glm::vec3(1, 0, 0));
	TestRenderObject* testObj = new TestRenderObject(*material);
	scene->addRenderable(static_cast<pxengine::PxeRenderBase&>(*testObj));
	window->setScene(scene);
	window->setSwapInterval(1);
	while (!window->getShouldClose())
	{
		window->acquireGlContext();
		window->setWindowShown(true);
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window->drawScene();
		window->swapFrameBuffer();
		SDL_Event e;
		while (window->pollEvents(&e));
		window->releaseGlContext();
	}

	//std::thread win1(runWindow, engineBase, 500, 500, "Win 1", 1, 0, 0);
	//std::thread win2(runWindow, engineBase, 500, 500, "Win 2", 0, 1, 0);
	//win1.join();
	//win2.join();
	engineBase->shutdown();
	return 0;
}
