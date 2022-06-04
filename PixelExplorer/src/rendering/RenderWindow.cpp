#include "RenderWindow.h"

#include <assert.h>
#include <algorithm>
#include <math.h>

#include "RenderGlobal.h"
#include "Logger.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define MAINTHREADCHECK() { \
							if(std::this_thread::get_id() != _spawnThreadId) { \
								Logger::error(__FUNCTION__ " must be called from the thread that created the window"); \
								return; \
							}\
						}

thread_local ImGuiContext* MyImGuiTLS;

namespace pixelexplorer::rendering {
	RenderWindow::RenderWindow(int32_t width, int32_t height, const char* title)
	{
		global::windowCreationLock.lock();
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
		global::windowCreationLock.unlock();
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

		// Init imgui
		IMGUI_CHECKVERSION();
		_guiContext = ImGui::CreateContext();
		ImGui::SetCurrentContext(_guiContext);
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		ImGui_ImplGlfw_InitForOpenGL(_window, true);
		// Should not hard code the version
		ImGui_ImplOpenGL3_Init("#version 150");
	}

	RenderWindow::~RenderWindow()
	{
		if (std::this_thread::get_id() != _spawnThreadId)
			Logger::fatal(__FUNCTION__ " must be called from the thread that created the window");

		glfwMakeContextCurrent(_window);
		_renderObjectMutex.lock();
		_guiElementMutext.lock();
		_glQueueMutex.lock();

		for (auto i = _glCreationQueue.begin(); i != _glCreationQueue.end(); ++i)
			(*i)->drop();

		for (auto i = _glDeletionQueue.begin(); i != _glDeletionQueue.end(); ++i) {
			(*i)->destroyGLObjects(this);
			(*i)->drop();
		}

		for (auto i = _renderObjects.begin(); i != _renderObjects.end(); ++i) {
			if ((*i)->getHasGLObjects())
				(*i)->destroyGLObjects(this);
			(*i)->drop();
		}

		for (auto i = _guiElements.begin(); i != _guiElements.end(); ++i) {
			if ((*i)->getHasGLObjects())
				(*i)->destroyGLObjects(this);
			(*i)->drop();
		}

		ImGui::SetCurrentContext(_guiContext);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(_window);
		global::windowCreationLock.lock();
		if (--global::windowCount == 0) {
			glfwTerminate();
			global::glfwInit = false;
		}
		global::windowCreationLock.unlock();
		_renderObjectMutex.unlock();
		_guiElementMutext.unlock();
		_glQueueMutex.unlock();

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
		updateGLQueues();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawRenderObjects();
		glfwPollEvents();
		drawGui();
		glfwSwapBuffers(_window);
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
		_renderObjectMutex.lock();
		_renderObjects.push_front(renderObject);
		if (renderObject->requiresGLObjects()) {
			_glQueueMutex.lock();
			_glCreationQueue.push_front(renderObject);
			_glQueueMutex.unlock();
		}

		_renderObjectMutex.unlock();
	}

	void RenderWindow::removeRenderMesh(RenderObject* renderObject)
	{
		_renderObjectMutex.lock();
		bool drppedElement = false;
		for (auto it = _renderObjects.begin(); it != _renderObjects.end(); ++it) {
			if ((*it) == renderObject) {
				drppedElement = true;
				_renderObjects.erase(it);
				if (renderObject->getHasGLObjects()) {
					_glQueueMutex.lock();
					_glDeletionQueue.push_front(renderObject);
					_glQueueMutex.unlock();
				}
				else {
					renderObject->drop();
				}
				break;
			}
		}

		_renderObjectMutex.unlock();
		if (!drppedElement)
			Logger::warn("Attempted to remove Render Object that is not in the RenderWindow");
	}

	void RenderWindow::addGUIElement(GUIElement* element)
	{
		element->grab();
		_guiElementMutext.lock();
		bool insertedElement = false;
		for (auto it = _guiElements.begin(); it != _guiElements.end(); ++it) {
			if ((*it)->getZIndex() >= element->getZIndex()) {
				_guiElements.insert(it, element);
				insertedElement = true;
				break;
			}
		}

		if (!insertedElement)
			_guiElements.push_back(element);

		if (element->requiresGLObjects()) {
			_glQueueMutex.lock();
			_glCreationQueue.push_front(element);
			_glQueueMutex.unlock();
		}

		_guiElementMutext.unlock();
	}

	void RenderWindow::removeGUIElement(GUIElement* element)
	{
		_guiElementMutext.lock();
		bool drppedElement = false;
		for (auto it = _guiElements.begin(); it != _guiElements.end(); ++it) {
			if ((*it) == element) {
				drppedElement = true;
				_guiElements.erase(it);
				if (element->getHasGLObjects()) {
					_glQueueMutex.lock();
					_glDeletionQueue.push_front(element);
					_glQueueMutex.unlock();
				}
				else {
					element->drop();
				}
				break;
			}
		}

		_guiElementMutext.unlock();
		if (!drppedElement)
			Logger::warn("Attempted to remove GUIElement that is not in the RenderWindow");
	}

	ImFont* RenderWindow::loadFont(const std::string& path)
	{
		if (std::this_thread::get_id() != _spawnThreadId) {
			Logger::error(__FUNCTION__ " must be called from the thread that created the window");
			return nullptr;
		}

		auto it = _loadedFonts.find(path);
		if (it != _loadedFonts.end())
			return it->second;

		ImGui::SetCurrentContext(_guiContext);
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 3;
		config.GlyphExtraSpacing.x = 1.0f;
		ImFont* font = io.Fonts->AddFontFromFileTTF(path.c_str(), 30, &config);
		if (font == nullptr)
			Logger::warn("Failed to load font " + path);

		_loadedFonts.emplace(path, font);
		return font;
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
			_windowHeight = height;
			_windowWidth = width;
			_projectionMatrix = glm::perspective(90.0f, _windowWidth / _windowHeight, 0.1f, 100.0f);
			glViewport(0, 0, width, height);
		}
	}

	void RenderWindow::glfwFocusCallback(bool focused)
	{
		Logger::debug("Window focused: " + std::to_string(focused));
	}

	void RenderWindow::updateGLQueues()
	{
		_glQueueMutex.lock();
		for (auto i = _glCreationQueue.begin(); i != _glCreationQueue.end(); ++i) {
			(*i)->createGLObjects(this);
		}

		_glCreationQueue.clear();
		for (auto i = _glDeletionQueue.begin(); i != _glDeletionQueue.end(); ++i) {
			(*i)->destroyGLObjects(this);
			(*i)->drop();
		}

		_glDeletionQueue.clear();
		_glQueueMutex.unlock();
	}

	void RenderWindow::drawRenderObjects()
	{
		glm::mat4 vp(_projectionMatrix * _viewMatrix);
		for (auto i = _renderObjects.begin(); i != _renderObjects.end(); ++i) {
			RenderObject* renderObj = *i;
			if (renderObj->meshVisible()) {
				Shader* shader = renderObj->getShader();
				if (shader == nullptr) {
					Logger::error("Render Object returned NULL Shader, Render Object removed");
					if (renderObj->getHasGLObjects()) {
						renderObj->destroyGLObjects(this);
					}

					renderObj->drop();
					i = _renderObjects.erase(i);
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
	}

	void RenderWindow::drawGui()
	{
		ImGui::SetCurrentContext(_guiContext);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//float widthScale = _windowWidth / 600;
		//float heightScale = _windowHeight / 400;
		float scale = fminf(_windowWidth / 600, _windowHeight / 400);
		_guiElementMutext.lock();
		for (auto it = _guiElements.begin(); it != _guiElements.end(); ++it)
			(*it)->drawElement(_windowWidth, _windowHeight, scale);
		_guiElementMutext.unlock();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}