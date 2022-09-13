#include <stdint.h>
#include <unordered_map>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <math.h>
#include <filesystem>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "CameraInterface.h"
#include "common/RefCount.h"
#include "common/Timer.h"
#include "RenderWindow.fwd.h"
#include "GLNode.h"
#include "GLObject.fwd.h"
#include "GLRenderObject.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"
#include "../input/InputManager.h"

#ifndef PIXELEXPLORE_ENGINE_RENDERING_RENDERWINDOW_H_
#define PIXELEXPLORE_ENGINE_RENDERING_RENDERWINDOW_H_
namespace pixelexplorer::engine::rendering {
	class RenderWindow : public RefCount
	{
	public:
		RenderWindow(int32_t width, int32_t height, const char* title, CameraInterface* camera = nullptr);
		virtual ~RenderWindow();
		bool shouldClose() const;
		void setShouldClose() const;
		void resetShouldClose() const;
		void drawFrame();
		Shader* getShader(const std::filesystem::path& path);
		ImFont* getFont(const std::filesystem::path& path);
		void registerGLObject(GLObject* obj);
		void addGLRenderObject(GLRenderObject* renderObject);
		void removeGLRenderObject(GLRenderObject* renderObject);
		void loadImGuiContext();
		void setActiveShader(const Shader* shader);
		void setModelMatrix(const glm::mat4& mtx);
		void setCamera(CameraInterface* camera);
		void resetDeltaTimer();

		inline double getDeltaTime() const { return _deltaTime; }

		inline uint32_t getWindowWidth() const { return _windowWidth; }

		inline uint32_t getWindowHeight() const { return _windowHeight; }

		inline float getWindowScale() const { return _windowScale; }

		inline Shader* getActiveShader() const { return _activeShader; }

		inline CameraInterface* getCamera() const { return _camera; }

		inline GLFWwindow* getRawWindow() const { return _window; }

		inline input::InputManager* getInputManager() const { return _inputManager; }

	private:
		friend class GLObject;
		friend class Shader;
		static void glfwStaticResizeCallback(GLFWwindow* window, int width, int height);
		static void glfwStaticFocusCallback(GLFWwindow* window, int focused);
		static void glfwStaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfwStaticCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void glfwStaticMouseScrollCallback(GLFWwindow* window, double xpos, double ypos);
		static void glfwStaticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		GLFWwindow* _window;
		input::InputManager* _inputManager;
		ImGuiContext* _guiContext;
		CameraInterface* _camera;
		Shader* _activeShader;
		GLNode<GLRenderObject> _glRenderObjects;
		GLNode<GLObject> _staticGLObjects;
		GLNode<GLObject> _staticGLObjectsRemoveQueue;
		std::unordered_map<std::string, Shader*> _loadedShaders;
		std::unordered_map<std::string, ImFont*> _loadedFonts;
		std::recursive_mutex _glRenderObjectsMutex;
		std::recursive_mutex _staticGLObjectsMutex;
		std::thread::id _spawnThreadId;
		uint32_t _windowWidth;
		uint32_t _windowHeight;
		Timer _deltaTimer;
		float _windowScale;
		double _deltaTime;
		bool _activatedGuiContext;

		void glfwResizeCallback(uint32_t width, uint32_t height);
		void glfwFocusCallback(bool focused);
		bool terminateGLObject(GLObject* glObject);
		bool terminateGLObjectUnsafe(GLObject* glObject);
		void removeShaderFromCache(Shader* shader);
		void updateGLQueues();
		void drawRenderObjects();
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

