#include "RenderWindow.h"

#include <assert.h>
#include <algorithm>

#include "RenderGlobal.h"
#include "Logger.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#define MAINTHREADCHECK() { \
							if(std::this_thread::get_id() != _spawnThreadId) { \
								Logger::error(__FUNCTION__ " must be called from the thread that created the window"); \
								return; \
							}\
						}

namespace pixelexplore::rendering {
	RenderWindow::RenderWindow(int32_t width, int32_t height, const char* title)
	{
		_spawnThreadId = std::this_thread::get_id();
		glfwSetErrorCallback(global::glfwErrorCallback);
		global::glfwInit = glfwInit();
		if (!global::glfwInit) {
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal("Failed to Init GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!_window)
		{
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal("Failed to Create Window");
		}

		++global::windowCount;
		Logger::debug("Window Created");
		glfwSetWindowUserPointer(_window, this);
		glfwMakeContextCurrent(_window);
		glfwSetWindowSizeCallback(_window, glfwStaticResizeCallback);
		glfwSetWindowFocusCallback(_window, glfwStaticFocusCallback);

		GLenum initCode = glewInit();
		if (initCode != GLEW_OK) {
			Logger::error(std::to_string(initCode) + ": " + (const char*)glewGetErrorString(initCode));
			Logger::fatal("Failed to Init GLEW");
		}

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(global::glErrorCallback, 0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		_viewMatrix = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		// this creates/update the projection matrix
		glfwResizeCallback(width, height);
	}

	RenderWindow::~RenderWindow()
	{
		if (std::this_thread::get_id() != _spawnThreadId)
			Logger::fatal(__FUNCTION__ " must be called from the thread that created the window");

		glfwMakeContextCurrent(_window);
		for (auto i = _addedRenderMeshes.begin(); i != _addedRenderMeshes.end(); ++i)
			(*i)->drop();

		for (auto i = _renderMeshes.begin(); i != _renderMeshes.end(); ++i) {
			(*i)->deleteGlObjects(this);
			(*i)->drop();
		}

		glfwDestroyWindow(_window);
		if (--global::windowCount == 0) {
			glfwTerminate();
			global::glfwInit = false;
		}

		Logger::debug("Window closed");
	}

	bool RenderWindow::shouldClose() const
	{
		return glfwWindowShouldClose(_window);
	}

	void RenderWindow::drawFrame()
	{
		MAINTHREADCHECK();
		glfwMakeContextCurrent(_window);

		_addRemoveRenderMeshMutex.lock();
		if (!_addedRenderMeshes.empty()) {
			for (auto i = _addedRenderMeshes.begin(); i != _addedRenderMeshes.end(); ++i) {
				_renderMeshes.push_front(*i);
				(*i)->createGlObjects(this);
			}

			_addedRenderMeshes.clear();
		}

		if (!_removedRenderMeshes.empty()) {
			for (auto it = _removedRenderMeshes.begin(); it != _removedRenderMeshes.end(); ++it) {
				bool droppedMesh = false;
				for (auto mesh = _renderMeshes.begin(); mesh != _renderMeshes.end(); ++mesh) {
					if (*it == *mesh) {
						(*mesh)->deleteGlObjects(this);
						(*mesh)->drop();
						_renderMeshes.erase(mesh);
						droppedMesh = true;
						break;
					}
				}

				if (!droppedMesh)
					Logger::warn("Attempted to remove RenderObject that is not in the RenderWindow");
			}

			_removedRenderMeshes.clear();
		}

		_addRemoveRenderMeshMutex.unlock();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 vp(_projectionMatrix * _viewMatrix);
		for (auto i = _renderMeshes.begin(); i != _renderMeshes.end(); ++i) {
			RenderObject* renderObj = *i;
			if (renderObj->meshVisible()) {
				Shader* shader = renderObj->getShader();
				if (shader == nullptr) {
					Logger::error("Render Object returned NULL Shader, Render Object removed");
					i = _renderMeshes.erase(i);
					continue;
				}

				shader->bind();
				Material* material = renderObj->getMaterial();
				if (material != nullptr)
					material->applyMaterial(shader);
				shader->setUniformm4fv("u_MVP", vp * renderObj->getPositionMatrix());
				renderObj->drawMesh();
			}
		}

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	Shader* RenderWindow::loadShader(std::string path)
	{
		if (std::this_thread::get_id() != _spawnThreadId) {
			Logger::error(__FUNCTION__ " must be called from the thread that created the window");
			return nullptr;
		}

		auto it = _loadedShaders.find(path);
		if (it != _loadedShaders.end()) {
			it->second->grab();
			return it->second;
		}

		Shader* shader = new Shader(path);
		if (!shader->isValid()) {
			shader->drop();
			return nullptr;
		}

		_loadedShaders.insert({ path, shader });
		shader->grab();
		return shader;
	}

	bool RenderWindow::dropShader(Shader* shader)
	{
		if (std::this_thread::get_id() != _spawnThreadId) {
			Logger::error(__FUNCTION__ " must be called from the thread that created the window");
			return false;
		}

		shader->drop();
		if (shader->getRefCount() == 1) {
			_loadedShaders.erase(shader->_path);
			assert(shader->drop());
			return true;
		}

		return false;
	}

	void RenderWindow::addRenderMesh(RenderObject* renderObject)
	{
		renderObject->grab();
		_addRemoveRenderMeshMutex.lock();
		_addedRenderMeshes.push_front(renderObject);
		_addRemoveRenderMeshMutex.unlock();
	}

	void RenderWindow::removeRenderMesh(RenderObject* renderObject)
	{
		_addRemoveRenderMeshMutex.lock();
		_removedRenderMeshes.push_front(renderObject);
		_addRemoveRenderMeshMutex.unlock();
	}

	void RenderWindow::glfwStaticResizeCallback(GLFWwindow* window, int width, int height)
	{
		((RenderWindow*)glfwGetWindowUserPointer(window))->glfwResizeCallback(width, height);
	}

	void RenderWindow::glfwStaticFocusCallback(GLFWwindow* window, int focused)
	{
		((RenderWindow*)glfwGetWindowUserPointer(window))->glfwFocusCallback(focused);
	}

	void RenderWindow::glfwResizeCallback(uint32_t width, uint32_t height)
	{
		Logger::debug("Window resized: " + std::to_string(width) + "X" + std::to_string(height));
		if (width != 0 && height != 0) {
			_projectionMatrix = glm::perspective(90.0f, (float)width / (float)height, 0.1f, 100.0f);
			glViewport(0, 0, width, height);
		}
	}

	void RenderWindow::glfwFocusCallback(bool focused)
	{
		Logger::debug("Window focused: " + std::to_string(focused));
	}
}