#include "InputManager.h"
#include "common/Logger.h"

namespace pixelexplorer::engine::input {
	InputManager::InputManager(GLFWwindow* window) : _cursorPosition(0), _scrollPosition(0), _cursorPositionChange(0), _scrollPositionChange(0), _pendingScrollPosition(0), _pendingCursorPosition(0) {
		_window = window;
		Logger::debug(__FUNCTION__" window input manager created");
	}

	InputManager::~InputManager() {
		auto actionIt = _actions.begin();
		while (actionIt != _actions.end())
		{
			for (InputAction* action : *((*actionIt).second))
				action->drop();
			delete (*actionIt).second;
			actionIt = _actions.erase(actionIt);
		}

		Logger::debug(__FUNCTION__" window input manager destroyed");
	}

	void InputManager::setWindow(GLFWwindow* window) {
		_window = window;
	}

	void InputManager::updatePositions() {
		_cursorPositionChange = _pendingCursorPosition - _cursorPosition;
		_cursorPosition = _pendingCursorPosition;
		_scrollPositionChange = _pendingScrollPosition - _scrollPosition;
		_scrollPosition = _pendingScrollPosition;
		setActionValue(InputSource{ InputSource::InputSourceType::CURSORPOS, PX_X_AXIS }, _cursorPositionChange.x);
		setActionValue(InputSource{ InputSource::InputSourceType::CURSORPOS, PX_Y_AXIS }, _cursorPositionChange.y);
		setActionValue(InputSource{ InputSource::InputSourceType::MOUSESCROLL, PX_X_AXIS }, _scrollPositionChange.x);
		setActionValue(InputSource{ InputSource::InputSourceType::MOUSESCROLL, PX_Y_AXIS }, _scrollPositionChange.y);
	}

	void InputManager::keyCallback(int key, int scancode, int action, int mods) {
		if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
		setActionValue(InputSource{ InputSource::InputSourceType::KEYBOARD, scancode }, (double)(action == GLFW_PRESS));
	}

	void InputManager::cursorPositionCallback(double xpos, double ypos) {
		_pendingCursorPosition = glm::dvec2(xpos, ypos);
	}

	void InputManager::mouseScrollCallback(double xpos, double ypos) {
		_pendingScrollPosition += glm::dvec2(xpos, ypos);
	}

	void InputManager::mouseButtonCallback(int button, int action, int mods) {
		if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
		setActionValue(InputSource{ InputSource::InputSourceType::MOUSEBUTTON, button }, (double)(action == GLFW_PRESS));
	}

	void InputManager::setActionValue(const InputSource& source, double value) {
		_actionMutex.lock_shared();
		uint64_t hash = source.getHash();
		auto foundAction = _actions.find(hash);
		if (foundAction == _actions.end()) {
			_actionMutex.unlock_shared();
			return;
		}

		std::set<InputAction*>* actionList = foundAction->second;
		for (auto it = actionList->begin(); it != actionList->end(); ++it)
			(*it)->setValue(value);
		_actionMutex.unlock_shared();
	}

	// Warning this function does NOT grab the action
	void InputManager::insertAction(const InputAction& action) {
		_actionMutex.lock();
		auto foundActionSet = _actions.find(action.getSource().getHash());
		if (foundActionSet == _actions.end())
			foundActionSet = _actions.emplace(action.getSource().getHash(), new std::set<InputAction*>()).first;
		foundActionSet->second->emplace((InputAction*)&action);
		_actionMutex.unlock();
	}

	// Warning this function does NOT drop the action
	void InputManager::removeAction(const InputAction& action) {
		_actionMutex.lock();
		auto foundActionSet = _actions.find(action.getSource().getHash());
		if (foundActionSet == _actions.end()) {
			_actionMutex.unlock();
			return;
		}

		std::set<InputAction*>* actionList = foundActionSet->second;
		actionList->erase((InputAction*)&action);
		if (actionList->empty()) {
			delete actionList;
			_actions.erase(foundActionSet);
		}

		_actionMutex.unlock();
	}

	InputAction* InputManager::findAction(const std::string& name) {
		_actionMutex.lock_shared();
		for (auto& actionPair : _actions) {
			for (InputAction* action : *(actionPair.second)) {
				if (action->getName() == name) {
					_actionMutex.unlock_shared();
					return action;
				}
			}
		}

		_actionMutex.unlock_shared();
		return nullptr;
	}

	InputAction* InputManager::getOrCreateAction(const std::string& name, const InputSource& defaultSource) {
		InputAction* action = findAction(name);
		if (!action) {
			action = new InputAction(name, defaultSource);
			insertAction(*action);
			Logger::debug(__FUNCTION__" created new action " + name);
		}

		return action;
	}

	void InputManager::setActionSource(InputAction& action, const InputSource& source) {
		if (action.getSource().getFullHash() == source.getFullHash()) return;
		removeAction(action);
		action._source = source;
		insertAction(action);
	}
}