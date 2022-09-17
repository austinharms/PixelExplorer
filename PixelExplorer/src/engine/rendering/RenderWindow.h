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

//#define PX_LOG_RENDER_FPS
#define PX_CHECK_CALLING_THREAD

#ifndef PIXELEXPLORE_ENGINE_RENDERING_RENDERWINDOW_H_
#define PIXELEXPLORE_ENGINE_RENDERING_RENDERWINDOW_H_
namespace pixelexplorer::engine::rendering {
	class RenderWindow : public RefCount
	{
	public:
		// RenderWindow's are used for drawing GLRenderObjects and managing GLObject
		// they also have an InputManager for helping with user/window input
		RenderWindow(int32_t width, int32_t height, const char* title, CameraInterface* camera = nullptr);
		virtual ~RenderWindow();

		// returns if the user wants to close the window (x button, alt F4, ...)
		bool shouldClose() const;

		// simulate the user wanting to close the window
		void setShouldClose() const;

		// clears the window close flag
		void resetShouldClose() const;

		// draws the next frame
		// this draws the frame, swaps frame buffers, and update the InputManager
		void drawFrame();

		// returns a shader object or null
		// will return cached shader objects if a shader with the same path already exist
		// warning this will not verify that the shader is valid or even if the file exist 
		Shader* loadShader(const std::filesystem::path& path);

		// returns a font object or null if it fails to load the font
		// will return cached font objects if a font with the same path already exist
		ImFont* loadFont(const std::filesystem::path& path);

		// add a GLObject to be tracked by the window
		// a GLObject my only be tracked by one window at a time and you can not unregister a GLObject
		// once a GLObject is resisted to a window it will only be unregistered if the window is destroyed
		void registerGLObject(GLObject& obj);

		// add a GLRenderObject to the window's render queue
		// this will also call registerGLObject on the object if it is not already registered
		void addGLRenderObject(GLRenderObject& renderObject);

		// remove a GLRenderObject from the window's render queue
		// this will not unregister the GLRenderObject
		void removeGLRenderObject(GLRenderObject& renderObject);

		// set the window's ImGui context current to allow use of ImGui functions
		// this will also force the window to draw ImGui on the next or current drawFrame call
		// ImGui may not be rendered if this is not called
		void setImGuiContexCurrent();

		// set the shader used for rendering GLRenderObjects
		// this method assumes that the shader is valid
		void setActiveShader(const Shader& shader);

		// if the shader is the current active shader, it is unbound
		// useful when dropping shader's
		void clearActiveShader(const Shader& shader);

		// set the u_ModelMatrix uniform on the active shader
		// returns true if the active shader is not null
		bool setModelMatrix(const glm::mat4& mtx);

		// set the window camera
		void setCamera(CameraInterface* camera);

		// reset the internal frame delta timer
		void resetDeltaTimer();

		// get the delta time of the last drawFrame call
		inline double getDeltaTime() const { return _deltaTime; }

		// returns the width of the window in pixels
		// note this will not be 0 when minimized
		inline uint32_t getWindowWidth() const { return _windowWidth; }

		// returns the height of the window in pixels
		// note this will not be 0 when minimized
		inline uint32_t getWindowHeight() const { return _windowHeight; }

		// returns the "scale" that GUI elements should use bases of the window reference size (600x400)
		inline float getWindowScale() const { return _windowScale; }

		// returns the shader currently being used to render GLRenderObjects
		inline Shader* getActiveShader() const { return _activeShader; }

		// returns the current CameraInterface that is being used
		inline CameraInterface* getCamera() const { return _camera; }

		// returns RenderWindow's the underlaying GLFWWindow object
		inline GLFWwindow& getRawWindow() const { return *_window; }

		// returns the InputManager for this window
		inline input::InputManager& getInputManager() const { return _inputManager; }

		// returns true if the calling thread id matches the id of the thread used to create the RenderWindow
		inline bool usingRenderThread() const { return std::this_thread::get_id() == _spawnThreadId; }

		// returns the id of the thread used to create the RenderWindow
		inline const std::thread::id& getRenderThreadId() const { return _spawnThreadId; }

	private:
		friend class GLObject;
		friend class Shader;
		static void glfwStaticResizeCallback(GLFWwindow* window, int width, int height);
		static void glfwStaticFocusCallback(GLFWwindow* window, int focused);
		static void glfwStaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void glfwStaticCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void glfwStaticMouseScrollCallback(GLFWwindow* window, double xpos, double ypos);
		static void glfwStaticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		// keep this a pointer because we need to init glfw before we can create the window, but you can assume its not null (when its null the program will exit with non 0 code)
		GLFWwindow* _window;
		// same as GLFWwindow*, keep it a pointer because it depends on other things, but you can assume its not null
		ImGuiContext* _guiContext;
		input::InputManager& _inputManager;
		CameraInterface* _camera;
		Shader* _activeShader;
		GLNode<GLRenderObject> _glRenderObjects;
		GLNode<GLObject> _staticGLObjects;
		GLNode<GLObject> _staticGLObjectsRemoveQueue;
		// allow null because we don't want to keep attempting to load an invalid/missing shader
		std::unordered_map<std::string, Shader*> _loadedShaders;
		// allow null because we don't want to keep attempting to load an invalid/missing font
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

		// unregister the GLObject from the window
		// if usingRenderThread returns true it will call terminateGLObjectUnsafe
		// else it will add the object to _staticGLObjectsRemoveQueue
		bool terminateGLObject(GLObject& glObject);

		// unregister the GLObject from the window immediately without putting it in the queue
		bool terminateGLObjectUnsafe(GLObject& glObject);

		// remove the shader from _loadedShaders map
		void removeShaderFromCache(Shader& shader);

		// used internally by drawFrame to process changes to _staticGLObjectsRemoveQueue
		void updateGLQueues();

		// used internally by drawFrame to render all of the GLRenderObjects in _glRenderObjects
		void drawRenderObjects();
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

