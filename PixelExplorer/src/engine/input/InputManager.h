#include <stdint.h>
#include <unordered_map>
#include <set>
#include <shared_mutex>

#include "../rendering/RenderWindow.fwd.h"
#include "common/RefCount.h"
#include "GLFW/glfw3.h"
#include "InputAction.h"
#include "InputSource.h"
#include "glm/glm/ext/vector_double2.hpp"

#ifndef PIXELEXPLORER_ENGINE_INPUT_INPUTMANAGER_H_
#define PIXELEXPLORER_ENGINE_INPUT_INPUTMANAGER_H_
namespace pixelexplorer::engine::input {
	class InputManager : public RefCount
	{
	public:
		virtual ~InputManager();
		//  Return an action with the same name, if no action if found null is returned, this should not be called every frame due to high overhead
		InputAction* findAction(const std::string& name);
		// Create and return a new InputAction, if an action with the same name is found that action will be returned instead, this should not be called every frame due to high overhead
		InputAction* getOrCreateAction(const std::string& name, const InputSource& defaultSource);
		// Set an action's source, this should not be called every frame due to high overhead
		void setActionSource(InputAction* action, const InputSource& source);

		inline const glm::dvec2& getCursorPos() const { return _cursorPosition; }

		inline const glm::dvec2& getScrollPos() const { return _scrollPosition; }

		inline const glm::dvec2& getCursorPositionChange() const { return _cursorPositionChange; }

		inline const glm::dvec2& getScrollPositionChange() const { return _scrollPositionChange; }

	private:
		friend class rendering::RenderWindow;
		GLFWwindow* _window;
		std::unordered_map<uint64_t, std::set<InputAction*>*> _actions;
		std::shared_mutex _actionMutex;
		glm::dvec2 _cursorPosition;
		glm::dvec2 _scrollPosition;
		glm::dvec2 _cursorPositionChange;
		glm::dvec2 _scrollPositionChange;
		glm::dvec2 _pendingScrollPosition;
		glm::dvec2 _pendingCursorPosition;

		InputManager(GLFWwindow* window = nullptr);
		void setWindow(GLFWwindow* window);
		void updatePositions();
		void keyCallback(int key, int scancode, int action, int mods);
		void cursorPositionCallback(double xpos, double ypos);
		void mouseScrollCallback(double xpos, double ypos);
		void mouseButtonCallback(int button, int action, int mods);
		void setActionValue(const InputSource& source, double value);
		// Warning this function does NOT grab the action
		void insertAction(InputAction* action);
		// Warning this function does NOT drop the action
		void removeAction(InputAction* action);
	};
}
#endif // !PIXELEXPLORER_ENGINE_INPUTMANAGER_H_
