#include "NpInputManager.h"

#include <new>

#include "NpLogger.h"
#include "NpAction.h"
#include "NpActionSource.h"
#include "SDL_events.h"
#include "NpEngine.h"
#include "backends/imgui_impl_sdl.h"

namespace pxengine {
	namespace nonpublic {
		PXE_NODISCARD PxeActionSource* NpInputManager::getLastActionSource()
		{
			if (!_lastActionCode) return nullptr;
			return getActionSource(_lastActionCode);
		}

		PXE_NODISCARD PxeActionSource* NpInputManager::getActionSource(PxeActionSourceCode sourceCode)
		{
			auto sourceItr = _sources.find(sourceCode);
			if (sourceItr != _sources.end()) return sourceItr->second;
			NpActionSource* source = new(std::nothrow) NpActionSource(_lastActionCode);
			if (!source) {
				PXE_ERROR("Failed to create PxeActionSource");
				return nullptr;
			}

			_sources.emplace(sourceCode, source);
			return source;
		}

		PXE_NODISCARD PxeAction* NpInputManager::getAction(const std::string& actionName)
		{
			auto it = _actions.find(actionName);
			if (it != _actions.end()) return it->second;
			PxeAction* action = new(std::nothrow) NpAction(actionName);
			if (!action) {
				PXE_ERROR("Failed to create PxeAction");
				return nullptr;
			}

			action->grab();
			_actions.emplace(actionName, action);
			return action;
		}

		PXE_NODISCARD int32_t NpInputManager::getCursorXPos() const
		{
			return _cursorPos.x;
		}

		PXE_NODISCARD int32_t NpInputManager::getCursorYPos() const
		{
			return _cursorPos.y;
		}

		PXE_NODISCARD int32_t NpInputManager::getCursorXChange() const
		{
			return _cursorChange.x;
		}

		PXE_NODISCARD int32_t NpInputManager::getCursorYChange() const
		{
			return _cursorChange.y;
		}

		PXE_NODISCARD const glm::i32vec2& NpInputManager::getCursorPos() const
		{
			return _cursorPos;
		}

		PXE_NODISCARD const glm::i32vec2& NpInputManager::getCursorChange() const
		{
			return _cursorChange;
		}

		PXE_NODISCARD PxeWindow* NpInputManager::getMouseFocusedWindow() const
		{
			return static_cast<PxeWindow*>(_focusedMouseWindow);
		}

		PXE_NODISCARD PxeWindow* NpInputManager::getKeyboardFocusedWindow() const
		{
			return static_cast<PxeWindow*>(_focusedKeyboardWindow);
		}

		void NpInputManager::clearLastActionSource()
		{
			_lastActionCode = 0;
		}

		NpInputManager::NpInputManager()
		{
			_lastActionCode = 0;
			_focusedKeyboardWindow = nullptr;
			_focusedMouseWindow = nullptr;
		}

		NpInputManager::~NpInputManager()
		{
			for (auto actionPair : _actions)
				actionPair.second->drop();
			_actions.clear();
			for (auto sourcePair : _sources)
				sourcePair.second->drop();
			_sources.clear();
		}

		void NpInputManager::processEvents()
		{
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				switch (evt.type)
				{
				case SDL_QUIT:
				{
					const std::unordered_map<uint32_t, NpWindow*>& windows = NpEngine::getInstance().getWindows();
					for (auto winPair : windows)
						winPair.second->setShouldClose();
				}
				break;

				case SDL_WINDOWEVENT:
				{
					const std::unordered_map<uint32_t, NpWindow*>& windows = NpEngine::getInstance().getWindows();
					auto windowItr = windows.find(evt.window.windowID);
					NpWindow* window = nullptr;
					if (windowItr != windows.end() && windowItr->second->getAssetStatus() == PxeGLAssetStatus::INITIALIZED) window = windowItr->second;
					switch (evt.window.event)
					{
					case SDL_WINDOWEVENT_ENTER:
						_focusedMouseWindow = window;
						break;

					case SDL_WINDOWEVENT_LEAVE:
						if (_focusedMouseWindow == window)
							_focusedMouseWindow = nullptr;
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						_focusedKeyboardWindow = window;
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
						if (_focusedKeyboardWindow == window)
							_focusedKeyboardWindow = nullptr;
						break;

					case SDL_WINDOWEVENT_CLOSE:
						if (window)
							window->setShouldClose();
						break;
					}

					if (window) {
						window->bindGuiContext();
						ImGui_ImplSDL2_ProcessEvent(&evt);
					}
				}
				break;

				case SDL_MOUSEMOTION:
					if (_focusedMouseWindow) {
						_focusedMouseWindow->bindGuiContext();
						ImGui_ImplSDL2_ProcessEvent(&evt);
						if (!ImGui::GetIO().WantCaptureMouse) {
							glm::i32vec2 mousePos(evt.motion.xrel, evt.motion.yrel);
							_cursorChange = mousePos - _cursorPos;
							_cursorPos = mousePos;
						}
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					if (_focusedMouseWindow) {
						_focusedMouseWindow->bindGuiContext();
						ImGui_ImplSDL2_ProcessEvent(&evt);
						if (!ImGui::GetIO().WantCaptureMouse) {
							PxeActionSourceCode code = ((uint64_t)PxeActionSourceType::MOUSE << 32) | (uint32_t)evt.button.button;
							if (evt.button.state)
								_lastActionCode = code;
							auto sourceItr = _sources.find(code);
							if (sourceItr != _sources.end())
								static_cast<NpActionSource*>(sourceItr->second)->setValue(evt.button.state);
						}
					}
					break;

				case SDL_MOUSEWHEEL:
					if (_focusedMouseWindow) {
						_focusedMouseWindow->bindGuiContext();
						ImGui_ImplSDL2_ProcessEvent(&evt);
					}
					break;

				case SDL_TEXTINPUT:
					if (_focusedKeyboardWindow) {
						_focusedKeyboardWindow->bindGuiContext();
						ImGui_ImplSDL2_ProcessEvent(&evt);
					}
					break;

				case SDL_KEYDOWN:
				case SDL_KEYUP:
					if (_focusedKeyboardWindow) {
						_focusedKeyboardWindow->bindGuiContext();
						ImGui_ImplSDL2_ProcessEvent(&evt);
						if (!ImGui::GetIO().WantCaptureKeyboard) {
							PxeActionSourceCode code = ((uint64_t)PxeActionSourceType::KEYBOARD << 32) | (uint32_t)evt.key.keysym.sym;
							if (evt.key.type == SDL_KEYDOWN)
								_lastActionCode = code;
							auto sourceItr = _sources.find(code);
							if (sourceItr != _sources.end())
								static_cast<NpActionSource*>(sourceItr->second)->setValue(evt.key.type == SDL_KEYDOWN);
						}
					}
					break;

				case SDL_JOYBUTTONUP:
				case SDL_JOYBUTTONDOWN:
				{
					PxeActionSourceCode code = ((uint64_t)PxeActionSourceType::JOYSTICK_BUTTON << 32) | (uint32_t)evt.jbutton.button;
					if (evt.jbutton.state)
						_lastActionCode = code;
					auto sourceItr = _sources.find(code);
					if (sourceItr != _sources.end())
						static_cast<NpActionSource*>(sourceItr->second)->setValue(evt.jbutton.state);
				}
				break;

				case SDL_JOYHATMOTION:
				{
					for (uint8_t hatDir = 1; hatDir < 0x10; hatDir = hatDir << 1) {
						PxeActionSourceCode code = ((uint64_t)PxeActionSourceType::JOYSTICK_HAT << 32) | ((uint64_t)evt.jhat.hat << 8) | hatDir;
						if (evt.jhat.value & hatDir)
							_lastActionCode = code;
						auto sourceItr = _sources.find(code);
						if (sourceItr != _sources.end())
							static_cast<NpActionSource*>(sourceItr->second)->setValue(evt.jhat.value & hatDir);
					}
				}
				break;

				case SDL_CONTROLLERBUTTONUP:
				case SDL_CONTROLLERBUTTONDOWN:
				{
					PxeActionSourceCode code = ((uint64_t)PxeActionSourceType::CONTROLLER_BUTTON << 32) | (uint32_t)evt.cbutton.button;
					if (evt.cbutton.state)
						_lastActionCode = code;
					auto sourceItr = _sources.find(code);
					if (sourceItr != _sources.end())
						static_cast<NpActionSource*>(sourceItr->second)->setValue(evt.cbutton.state);
				}
				break;
				}
			}
		}

		void NpInputManager::clearActiveWindow(NpWindow* window)
		{
			if (window) {
				if (_focusedKeyboardWindow == window)
					_focusedKeyboardWindow = nullptr;
				if (_focusedMouseWindow == window)
					_focusedMouseWindow = nullptr;
			}
			else {
				_focusedMouseWindow = nullptr;
				_focusedKeyboardWindow = nullptr;
			}
		}
	}
}
