#define LOG_RENDER_FPS

#include "RenderWindow.h"

#include <assert.h>
#include <algorithm>
#ifdef LOG_RENDER_FPS
#include <chrono>
#endif

#include "GLObject.h"
#include "RenderGlobal.h"
#include "Logger.h"
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
	RenderWindow::RenderWindow(int32_t width, int32_t height, const char* title, CameraInterface* camera)
	{
		_activatedGuiContext = false;
		_activeShader = nullptr;
		_camera = nullptr;
		global::windowCreationLock.lock();
		_spawnThreadId = std::this_thread::get_id();
		glfwSetErrorCallback(global::glfwErrorCallback);
		global::glfwInit = glfwInit();
		if (!global::glfwInit) {
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal(__FUNCTION__" Failed to Init GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!_window)
		{
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal(__FUNCTION__" Failed to Create Window");
		}

		++global::windowCount;
		global::windowCreationLock.unlock();
		Logger::debug(__FUNCTION__" Window Created");
		glfwSetWindowUserPointer(_window, this);
		glfwMakeContextCurrent(_window);
		glfwSetWindowSizeCallback(_window, glfwStaticResizeCallback);
		glfwSetWindowFocusCallback(_window, glfwStaticFocusCallback);
		glfwSwapInterval(0);

		GLenum initCode = glewInit();
		if (initCode != GLEW_OK) {
			Logger::error(std::to_string(initCode) + ": " + (const char*)glewGetErrorString(initCode));
			Logger::fatal(__FUNCTION__" Failed to Init GLEW");
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
		setCamera(camera);
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
		// TODO fix hard coding the version
		ImGui_ImplOpenGL3_Init("#version 150");
	}

	RenderWindow::~RenderWindow()
	{
		if (std::this_thread::get_id() != _spawnThreadId)
			Logger::fatal(__FUNCTION__ " Render window destructor must be called from the thread that created the window");

		glfwMakeContextCurrent(_window);
		_glRenderObjectsMutex.lock();
		_staticGLObjectsMutex.lock();

		if (_activeShader != nullptr) {
			_activeShader->unbind();
			_activeShader->drop();
			_activeShader = nullptr;
		}

		if (_camera != nullptr) {
			_camera->setActive(false);
			_camera->drop();
			_camera = nullptr;
		}

		{
			GLNode<GLRenderObject>* currentNode = _glRenderObjects.next;
			while (currentNode != nullptr) {
				GLRenderObject* renderObject = static_cast<GLRenderObject*>(currentNode);
				currentNode = currentNode->next;
				removeGLRenderObject(renderObject);
			}
		}

		{
			GLNode<GLObject>* currentNode = _staticGLObjects.next;
			while (currentNode != nullptr) {
				GLObject* glObj = static_cast<GLObject*>(currentNode);
				currentNode = currentNode->next;
				terminateGLObjectUnsafe(glObj);
			}
		}

		{
			GLNode<GLObject>* currentNode = _staticGLObjectsRemoveQueue.next;
			while (currentNode != nullptr) {
				GLObject* glObj = static_cast<GLObject*>(currentNode);
				currentNode = currentNode->next;
				terminateGLObjectUnsafe(glObj);
			}
		}

		ImGui::SetCurrentContext(_guiContext);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		_guiContext = nullptr;

		glfwDestroyWindow(_window);
		global::windowCreationLock.lock();
		if (--global::windowCount == 0) {
			glfwTerminate();
			global::glfwInit = false;
		}

		global::windowCreationLock.unlock();
		_staticGLObjectsMutex.unlock();
		_glRenderObjectsMutex.unlock();
		Logger::debug("Window closed");
	}

	bool RenderWindow::shouldClose() const
	{
		return glfwWindowShouldClose(_window);
	}

	void RenderWindow::drawFrame()
	{
#ifdef LOG_RENDER_FPS
		auto start = std::chrono::high_resolution_clock::now();
#endif
		MAINTHREADCHECK();
		glfwMakeContextCurrent(_window);
		updateGLQueues();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
		drawRenderObjects();
		glfwSwapBuffers(_window);
#ifdef LOG_RENDER_FPS
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		float ms = ((float)duration.count()) / 1000.0f;
		Logger::debug("Frame took: " + std::to_string(ms) + "ms, FPS: " + std::to_string((1.0f / ms) * 1000));
#endif
	}

	Shader* RenderWindow::getShader(std::string path)
	{
		auto it = _loadedShaders.find(path);
		if (it != _loadedShaders.end()) {
			it->second->grab();
			return it->second;
		}

		Shader* shader = new Shader(path);
		_staticGLObjectsMutex.lock();
		registerGLObject(shader);
		_loadedShaders.insert({ path, shader });
		_staticGLObjectsMutex.unlock();
		return shader;
	}

	void RenderWindow::addGLRenderObject(GLRenderObject* renderObject)
	{
		if (renderObject->getAttached()) {
			if (renderObject->getRenderWindow() != this) {
				Logger::error(__FUNCTION__" Attempted to add GLRenderObject already attached to different Window, GLRenderObject not added");
				return;
			}
		}
		else {
			registerGLObject(renderObject);
		}

		if (renderObject->inRenderQueue()) {
			Logger::warn(__FUNCTION__" Attempted to add GLRenderObject already in render queue to render queue");
			return;
		}

		renderObject->grab();
		_glRenderObjectsMutex.lock();
		GLNode<GLRenderObject>* lastNode = &_glRenderObjects;
		GLNode<GLRenderObject>* node = _glRenderObjects.next;
		while (node != nullptr && static_cast<GLRenderObject*>(node)->getPriority() < renderObject->getPriority()) {
			lastNode = node;
			node = node->next;
		}

		static_cast<GLNode<GLRenderObject>*>(renderObject)->insertNodeAfter<GLRenderObject>(lastNode);
		_glRenderObjectsMutex.unlock();
	}

	void RenderWindow::removeGLRenderObject(GLRenderObject* renderObject) {
		if (renderObject->getAttached()) {
			if (renderObject->getRenderWindow() != this) {
				Logger::error(__FUNCTION__" Attempted to remove GLRenderObject attached to different Window, GLRenderObject not removed");
				return;
			}
		}
		else {
			Logger::error(__FUNCTION__" Attempted to remove GLRenderObject not attached to Window, GLRenderObject not removed");
			return;
		}

		if (!renderObject->inRenderQueue()) {
			Logger::warn(__FUNCTION__" Attempted to remove GLRenderObject not in render queue, GLRenderObject not removed");
			return;
		}

		_glRenderObjectsMutex.lock();
		static_cast<GLNode<GLRenderObject>*>(renderObject)->removeNode<GLRenderObject>();
		_glRenderObjectsMutex.unlock();
		renderObject->drop();
	}

	void RenderWindow::loadImGuiContext()
	{
		MAINTHREADCHECK();
		if (!_activatedGuiContext) {
			_activatedGuiContext = true;
			ImGui::SetCurrentContext(_guiContext);
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
	}

	void RenderWindow::setActiveShader(const Shader* shader)
	{
		MAINTHREADCHECK();
		if (shader == nullptr) {
			Logger::error("Failed to set RenderWindow shader, shader was NULL");
			return;
		}

		_activeShader = (Shader*)shader;
		_activeShader->bind();
		if (_camera != nullptr)
			_activeShader->setUniformm4fv("u_ViewProjectionMatrix", _camera->getVPMatrix());
	}

	void RenderWindow::setModelMatrix(const glm::mat4& mtx)
	{
		MAINTHREADCHECK();
		if (_activeShader != nullptr)
			_activeShader->setUniformm4fv("u_ModelMatrix", mtx);
	}

	void RenderWindow::setCamera(CameraInterface* camera) {
		if (_camera != nullptr) {
			_camera->setActive(false);
			_camera->drop();
		}

		if ((_camera = camera) != nullptr) {
			_camera->grab();
			_camera->setActive(true);
			_camera->windowResize(_windowWidth, _windowHeight);
		}
	}

	bool RenderWindow::terminateGLObject(GLObject* glObject)
	{
		if (glObject->getRenderWindow() != this) {
			Logger::error("Attempted to remove RenderObject that is not in the RenderWindow");
			return false;
		}

		std::lock_guard<std::recursive_mutex> lock(_staticGLObjectsMutex);
		glObject->removeNode<GLObject>();
		if (std::this_thread::get_id() == _spawnThreadId) {
			return terminateGLObjectUnsafe(glObject);
		}
		else {
			glObject->insertNodeAfter(&_staticGLObjectsRemoveQueue);
			return false;
		}
	}

	bool RenderWindow::terminateGLObjectUnsafe(GLObject* glObject)
	{
		glObject->removeNode<GLObject>();
		glObject->terminate();
		return glObject->drop();
	}

	void RenderWindow::removeShaderFromCache(Shader* shader)
	{
		_staticGLObjectsMutex.lock();
		if (_loadedShaders.erase(shader->_path) == 0)
			Logger::warn("Failed to remove " + shader->_path + " from RenderWindow Shader cache");
		_staticGLObjectsMutex.unlock();
	}

	ImFont* RenderWindow::getFont(const std::string& path)
	{
		if (std::this_thread::get_id() != _spawnThreadId) {
			Logger::warn(__FUNCTION__ " must be called from the thread that created the window");
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
			Logger::error(__FUNCTION__" Failed to load font " + path);

		_loadedFonts.emplace(path, font);
		return font;
	}

	void RenderWindow::registerGLObject(GLObject* obj)
	{
		if (obj->getAttached()) {
			if (obj->getRenderWindow() != this)
				Logger::error(__FUNCTION__" Attempted to register GLObject already attached to different Window, GLObject not register");
			return;
		}

		obj->grab();
		obj->attach(this);
		_staticGLObjectsMutex.lock();
		obj->insertNodeAfter<GLObject>(&_staticGLObjects);
		_staticGLObjectsMutex.unlock();
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
		Logger::debug(__FUNCTION__" Window resized: " + std::to_string(width) + "X" + std::to_string(height));
		if (width != 0 && height != 0) {
			_windowHeight = height;
			_windowWidth = width;
			glViewport(0, 0, width, height);
			_windowScale = fminf((float)_windowWidth / 600.0f, (float)_windowHeight / 400.0f);
			if (_camera != nullptr)
				_camera->windowResize(width, height);
		}
	}

	void RenderWindow::glfwFocusCallback(bool focused)
	{
		Logger::debug(__FUNCTION__" Window focused: " + std::to_string(focused));
	}

	void RenderWindow::updateGLQueues()
	{
		if (_staticGLObjectsRemoveQueue.next != nullptr) {
			_staticGLObjectsMutex.lock();
			GLNode<GLObject>* currentNode = _staticGLObjectsRemoveQueue.next;
			while (currentNode != nullptr) {
				GLObject* glObj = static_cast<GLObject*>(currentNode);
				currentNode = currentNode->next;
				terminateGLObjectUnsafe(glObj);
			}

			_staticGLObjectsMutex.unlock();
		}
	}

	void RenderWindow::drawRenderObjects()
	{
		if (_camera == nullptr) {
			Logger::warn(__FUNCTION__" attempted to render with null camera, frame not rendered");
			return;
		}

		GLNode<GLRenderObject>* node = _glRenderObjects.next;
		while (node != nullptr)
		{
			static_cast<GLRenderObject*>(node)->update();
			node = node->next;
		}

		if (_activatedGuiContext) {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			_activatedGuiContext = false;
		}

		if (_activeShader != nullptr) {
			_activeShader->unbind();
			_activeShader = nullptr;
		}
	}
}