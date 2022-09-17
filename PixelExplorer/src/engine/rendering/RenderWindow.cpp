#include "RenderWindow.h"

#include <assert.h>
#include <algorithm>
#include <chrono>

#include "GLObject.h"
#include "RenderGlobal.h"
#include "common/Logger.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef PX_CHECK_CALLING_THREAD
#define PX_RENDERER_CHECK_THREAD_DEFAULT_RETURN() do { if(!usingRenderThread()) { Logger::error(__FUNCTION__ " must be called from the thread that created the window"); return; }} while(0)
#define PX_RENDERER_CHECK_THREAD_CUSTOM_RETURN(RETURN_VALUE) do { if(!usingRenderThread()) { Logger::error(__FUNCTION__ " must be called from the thread that created the window"); return RETURN_VALUE; }} while(0)
#define PX_RENDERER_CHECK_THREAD_X(A,FUNC, ...)  FUNC
#define PX_RENDERER_CHECK_THREAD(...)  PX_RENDERER_CHECK_THREAD_X(,##__VA_ARGS__,\
                                          PX_RENDERER_CHECK_THREAD_CUSTOM_RETURN(__VA_ARGS__),\
                                          PX_RENDERER_CHECK_THREAD_DEFAULT_RETURN(__VA_ARGS__)\
                                         ) 
#define PX_RENDERER_CHECK_THREAD_FATAL() do { if(!usingRenderThread()) Logger::fatal(__FUNCTION__" must be called from the thread that created the window"); } while(0)
#else
#define PX_RENDERER_CHECK_THREAD(...) do {} while(0)
#define PX_RENDERER_CHECK_THREAD_FATAL() do {} while(0)
#endif // PX_CHECK_CALLING_THREAD


//#define MAINTHREADCHECK() { \
//							if(std::this_thread::get_id() != _spawnThreadId) { \
//								Logger::error(__FUNCTION__ " must be called from the thread that created the window"); \
//								return; \
//							}\
//						}

thread_local ImGuiContext* MyImGuiTLS;

namespace pixelexplorer::engine::rendering {
	RenderWindow::RenderWindow(int32_t width, int32_t height, const char* title, CameraInterface* camera) : _inputManager(*(new input::InputManager()))
	{
		_spawnThreadId = std::this_thread::get_id();
		_activeShader = nullptr;
		_camera = nullptr;
		_activatedGuiContext = false;
		_deltaTime = 0;
		_windowWidth = width;
		_windowHeight = height;

		// this causes divide by 0 errors in the projection matrix and should just never happen
		if (width == 0 || height == 0)
			Logger::fatal(__FUNCTION__" attempted to create with 0 width or height");

		// register window with global object
		global::windowCreationLock.lock();
		glfwSetErrorCallback(global::glfwErrorCallback);
		global::glfwInit = glfwInit();
		if (!global::glfwInit) {
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal(__FUNCTION__" Failed to Init GLFW");
		}

		// create glfw window object
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

		// setup glfw callbacks
		glfwSetWindowUserPointer(_window, this);
		glfwMakeContextCurrent(_window);
		glfwSetWindowSizeCallback(_window, glfwStaticResizeCallback);
		glfwSetWindowFocusCallback(_window, glfwStaticFocusCallback);
		glfwSetCursorPosCallback(_window, glfwStaticCursorPosCallback);
		glfwSetScrollCallback(_window, glfwStaticMouseScrollCallback);
		glfwSetMouseButtonCallback(_window, glfwStaticMouseButtonCallback);
		glfwSetKeyCallback(_window, glfwStaticKeyCallback);
		// set window on input manager now that callbacks are setup
		_inputManager.setWindow(_window);
		// disable vsync (not guaranteed)
		glfwSwapInterval(0);

		// init glew on the new openGL context created by glfwCreateWindow
		GLenum initCode = glewInit();
		if (initCode != GLEW_OK) {
			Logger::error(std::to_string(initCode) + ": " + (const char*)glewGetErrorString(initCode));
			Logger::fatal(__FUNCTION__" Failed to Init GLEW");
		}

		// set openGL settings? I don't think that is the right word
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(global::glErrorCallback, 0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// this sets up the glViewport
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
		// this must be done after setting callbacks as this will override them (callbacks are called/forwarded by ImGUI)
		ImGui_ImplGlfw_InitForOpenGL(_window, true);
		// TODO fix hard coding the version
		ImGui_ImplOpenGL3_Init("#version 150");

		// needs to happen after _windowWidth and _windowHeight and set and are not 0
		setCamera(camera);
	}

	RenderWindow::~RenderWindow()
	{
		PX_RENDERER_CHECK_THREAD_FATAL();
		// this prevents the input manager from changing window properties
		_inputManager.setWindow(nullptr);

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
			GLNode<GLRenderObject>* currentNode = _glRenderObjects.Next;
			while (currentNode != nullptr) {
				GLRenderObject* renderObject = static_cast<GLRenderObject*>(currentNode);
				currentNode = currentNode->Next;
				removeGLRenderObject(*renderObject);
			}
		}

		{
			GLNode<GLObject>* currentNode = _staticGLObjects.Next;
			while (currentNode != nullptr) {
				GLObject* glObj = static_cast<GLObject*>(currentNode);
				currentNode = currentNode->Next;
				terminateGLObjectUnsafe(*glObj);
			}
		}

		{
			GLNode<GLObject>* currentNode = _staticGLObjectsRemoveQueue.Next;
			while (currentNode != nullptr) {
				GLObject* glObj = static_cast<GLObject*>(currentNode);
				currentNode = currentNode->Next;
				terminateGLObjectUnsafe(*glObj);
			}
		}

		_staticGLObjectsMutex.unlock();
		_glRenderObjectsMutex.unlock();
		ImGui::SetCurrentContext(_guiContext);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		_guiContext = nullptr;

		glfwDestroyWindow(_window);
		// this must not happen before glfw callbacks are removed
		_inputManager.drop();
		global::windowCreationLock.lock();
		if (--global::windowCount == 0) {
			glfwTerminate();
			global::glfwInit = false;
		}

		global::windowCreationLock.unlock();
		Logger::debug("Window closed");
	}

	bool RenderWindow::shouldClose() const
	{
		return glfwWindowShouldClose(_window);
	}

	void RenderWindow::setShouldClose() const
	{
		glfwSetWindowShouldClose(_window, true);
	}

	void RenderWindow::resetShouldClose() const
	{
		glfwSetWindowShouldClose(_window, false);
	}

	void RenderWindow::drawFrame()
	{
		PX_RENDERER_CHECK_THREAD();
#ifdef PX_LOG_RENDER_FPS
		Timer frameTimer;
#endif
		glfwMakeContextCurrent(_window);
		updateGLQueues();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawRenderObjects();
		glfwSwapBuffers(_window);
		glfwPollEvents();
		_inputManager.updatePositions();
		_deltaTime = _deltaTimer.elapsed();
		if (_deltaTime >= 1.0) {
			_deltaTime = 1;
			Logger::warn(__FUNCTION__" delta time over 1, limiting delta time to 1");
		}

		_deltaTimer.reset();
#ifdef PX_LOG_RENDER_FPS
		double frameTime = frameTimer.elapsed() * 1000.0;
		Logger::debug("Frame took: " + std::to_string(frameTime) + "ms, FPS: " + std::to_string((1.0f / frameTime) * 1000));
#endif
	}

	Shader* RenderWindow::loadShader(const std::filesystem::path& path)
	{
		std::lock_guard<std::recursive_mutex> lock(_staticGLObjectsMutex);
		auto it = _loadedShaders.find(path.string());
		if (it != _loadedShaders.end()) {
			it->second->grab();
			return it->second;
		}

		Shader* shader = new Shader(path.string());
		// this should only happen if there is memory allocation issues, should this be a fatal log?
		if (shader == nullptr) Logger::error(__FUNCTION__" failed to create shader object");
		if (shader != nullptr) registerGLObject(*shader);
		_loadedShaders.emplace(path.string(), shader);
		return shader;
	}

	void RenderWindow::addGLRenderObject(GLRenderObject& renderObject)
	{
		renderObject.grab();
		if (renderObject.getAttached()) {
			if (renderObject.getRenderWindow() != this) {
				Logger::error(__FUNCTION__" Attempted to add GLRenderObject already attached to different Window, GLRenderObject not added");
				renderObject.drop();
				return;
			}
		}
		else {
			registerGLObject(renderObject);
		}

		if (renderObject.inRenderQueue()) {
			Logger::warn(__FUNCTION__" Attempted to add GLRenderObject already in render queue to render queue");
			renderObject.drop();
			return;
		}

		_glRenderObjectsMutex.lock();
		GLNode<GLRenderObject>* lastNode = &_glRenderObjects;
		GLNode<GLRenderObject>* node = _glRenderObjects.Next;
		while (node != nullptr && static_cast<GLRenderObject*>(node)->getPriority() < renderObject.getPriority()) {
			lastNode = node;
			node = node->Next;
		}

		static_cast<GLNode<GLRenderObject>&>(renderObject).insertNodeAfter(lastNode);
		_glRenderObjectsMutex.unlock();
	}

	void RenderWindow::removeGLRenderObject(GLRenderObject& renderObject) {
		if (renderObject.getAttached()) {
			if (renderObject.getRenderWindow() != this) {
				Logger::error(__FUNCTION__" Attempted to remove GLRenderObject attached to different Window, GLRenderObject not removed");
				return;
			}
		}
		else {
			Logger::error(__FUNCTION__" Attempted to remove GLRenderObject not attached to Window, GLRenderObject not removed");
			return;
		}

		if (!renderObject.inRenderQueue()) {
			Logger::warn(__FUNCTION__" Attempted to remove GLRenderObject not in render queue, GLRenderObject not removed");
			return;
		}

		_glRenderObjectsMutex.lock();
		static_cast<GLNode<GLRenderObject>&>(renderObject).removeNode();
		_glRenderObjectsMutex.unlock();
		renderObject.drop();
	}

	void RenderWindow::setImGuiContexCurrent()
	{
		PX_RENDERER_CHECK_THREAD();
		if (!_activatedGuiContext) {
			_activatedGuiContext = true;
			ImGui::SetCurrentContext(_guiContext);
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
	}

	void RenderWindow::setActiveShader(const Shader& shader)
	{
		PX_RENDERER_CHECK_THREAD();
		// we are not grabbing the shader here for performance
		// also the shader should be grabbed by the windows shader cache
		_activeShader = (Shader*)&shader;
		shader.bind();
		if (_camera != nullptr)
			_activeShader->setUniformm4fv("u_ViewProjectionMatrix", _camera->getVPMatrix());
	}

	void rendering::RenderWindow::clearActiveShader(const Shader& shader)
	{
		PX_RENDERER_CHECK_THREAD();
		if (_activeShader && _activeShader == &shader) {
			_activeShader->unbind();
			// we don't grab the shader, so we don't drop it here
			_activeShader = nullptr;
		}
	}

	bool RenderWindow::setModelMatrix(const glm::mat4& mtx)
	{
		PX_RENDERER_CHECK_THREAD();
		if (_activeShader == nullptr) return false;
		_activeShader->setUniformm4fv("u_ModelMatrix", mtx);
		return true;
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

	void rendering::RenderWindow::resetDeltaTimer()
	{
		_deltaTimer.reset();
	}

	bool RenderWindow::terminateGLObject(GLObject& glObject)
	{
		if (glObject.getRenderWindow() != this) {
			Logger::error(__FUNCTION__" Attempted to remove RenderObject that is not in the RenderWindow");
			return false;
		}

		std::lock_guard<std::recursive_mutex> lock(_staticGLObjectsMutex);
		glObject.removeNode();
		if (usingRenderThread()) {
			return terminateGLObjectUnsafe(glObject);
		}
		else {
			glObject.insertNodeAfter(&_staticGLObjectsRemoveQueue);
			return false;
		}
	}

	bool RenderWindow::terminateGLObjectUnsafe(GLObject& glObject)
	{
		PX_RENDERER_CHECK_THREAD();
		glObject.removeNode();
		glObject.terminate();
		return glObject.drop();
	}

	void RenderWindow::removeShaderFromCache(Shader& shader)
	{
		_staticGLObjectsMutex.lock();
		if (_loadedShaders.erase(shader._path) == 0)
			Logger::warn(__FUNCTION__" failed to remove " + shader._path + " from window Shader cache");
		_staticGLObjectsMutex.unlock();
	}

	ImFont* RenderWindow::loadFont(const std::filesystem::path& path)
	{
		PX_RENDERER_CHECK_THREAD(nullptr);
		std::lock_guard<std::recursive_mutex> lock(_staticGLObjectsMutex);
		auto it = _loadedFonts.find(path.string());
		if (it != _loadedFonts.end())
			return it->second;

		ImGui::SetCurrentContext(_guiContext);
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 3;
		config.GlyphExtraSpacing.x = 1.0f;
		ImFont* font = io.Fonts->AddFontFromFileTTF(path.string().c_str(), 30, &config);
		if (font == nullptr) Logger::error(__FUNCTION__" Failed to load font " + path.string());
		_loadedFonts.emplace(path.string(), font);
		return font;
	}

	void RenderWindow::registerGLObject(GLObject& obj)
	{
		obj.grab();
		if (obj.getAttached()) {
			if (obj.getRenderWindow() != this)
				Logger::error(__FUNCTION__" Attempted to register GLObject already attached to different Window, GLObject not register");
			obj.drop();
			return;
		}

		obj.attach(*this);
		_staticGLObjectsMutex.lock();
		obj.insertNodeAfter(&_staticGLObjects);
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

	void RenderWindow::glfwStaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		input::InputManager& inputManager = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window))->_inputManager;
		inputManager.keyCallback(key, scancode, action, mods);
	}

	void rendering::RenderWindow::glfwStaticCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		input::InputManager& inputManager = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window))->_inputManager;
		inputManager.cursorPositionCallback(xpos, ypos);
	}

	void RenderWindow::glfwStaticMouseScrollCallback(GLFWwindow* window, double xpos, double ypos)
	{
		input::InputManager& inputManager = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window))->_inputManager;
		inputManager.mouseScrollCallback(xpos, ypos);
	}

	void RenderWindow::glfwStaticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		input::InputManager& inputManager = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window))->_inputManager;
		inputManager.mouseButtonCallback(button, action, mods);
	}

	void RenderWindow::glfwResizeCallback(uint32_t width, uint32_t height)
	{
		// will this ever fail? this should only ever be called by glfw
		PX_RENDERER_CHECK_THREAD();
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
		PX_RENDERER_CHECK_THREAD();
		if (_staticGLObjectsRemoveQueue.Next != nullptr) {
			_staticGLObjectsMutex.lock();
			GLNode<GLObject>* currentNode = _staticGLObjectsRemoveQueue.Next;
			while (currentNode != nullptr) {
				GLObject* glObj = static_cast<GLObject*>(currentNode);
				currentNode = currentNode->Next;
				terminateGLObjectUnsafe(*glObj);
			}

			_staticGLObjectsMutex.unlock();
		}
	}

	void RenderWindow::drawRenderObjects()
	{
		PX_RENDERER_CHECK_THREAD();
		if (_camera == nullptr) {
			Logger::warn(__FUNCTION__" attempted to render with null camera, frame not rendered");
			return;
		}

		GLNode<GLRenderObject>* node = _glRenderObjects.Next;
		while (node != nullptr)
		{
			static_cast<GLRenderObject*>(node)->update();
			node = node->Next;
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