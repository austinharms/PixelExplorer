#include "PxeInputManager.h"

#include <unordered_map>

#include "PxeLogger.h"
#include "PxeEngine.h"
#include "SDL_events.h"
#include "imgui/backends/imgui_impl_sdl.h"

namespace pxengine {
	struct PxeInputManager::Impl {
		PxeWindow* _focusedKeyboardWindow;
		PxeWindow* _focusedMouseWindow;
		PxeActionSourceCode _lastActionCode;
		std::unordered_map<PxeActionSourceCode, PxeActionSource*> _sources;
		std::unordered_map<std::string, PxeAction*> _actions;
		glm::i32vec2 _cursorPos;
		glm::i32vec2 _cursorChange;
	};

	PxeInputManager::PxeInputManager()
	{
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeInputManager::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeInputManager::STORAGE_ALIGN need be changed");

		new(&_storage) Impl();
		Impl& imp = impl();
		imp._lastActionCode = 0;
		imp._focusedKeyboardWindow = nullptr;
		imp._focusedMouseWindow = nullptr;
	}

	PxeInputManager::~PxeInputManager()
	{
		Impl& imp = impl();
		for (auto actionPair : imp._actions)
			actionPair.second->drop();
		imp._actions.clear();
		for (auto sourcePair : imp._sources)
			sourcePair.second->drop();
		imp._sources.clear();
		imp.~Impl();
	}

	PXE_NODISCARD PxeActionSource* PxeInputManager::getLastActionSource()
	{
		Impl& imp = impl();
		if (!imp._lastActionCode) return nullptr;
		return getActionSource(imp._lastActionCode);
	}

	PXE_NODISCARD PxeActionSource* PxeInputManager::getActionSource(PxeActionSourceCode sourceCode)
	{
		Impl& imp = impl();
		auto sourceItr = imp._sources.find(sourceCode);
		if (sourceItr != imp._sources.end()) return sourceItr->second;
		PxeActionSource* source = new(std::nothrow) PxeActionSource(imp._lastActionCode);
		if (!source) {
			PXE_ERROR("Failed to allocate PxeActionSource");
			return nullptr;
		}

		imp._sources.emplace(sourceCode, source);
		return source;
	}

	PXE_NODISCARD PxeAction* PxeInputManager::getAction(const std::string& actionName)
	{
		Impl& imp = impl();
		auto it = imp._actions.find(actionName);
		if (it != imp._actions.end()) return it->second;
		PxeAction* action = new(std::nothrow) PxeAction(actionName);
		if (!action) {
			PXE_ERROR("Failed to allocate PxeAction");
			return nullptr;
		}

		action->grab();
		imp._actions.emplace(actionName, action);
		return action;
	}

	PXE_NODISCARD int32_t PxeInputManager::getCursorXPos() const
	{
		return impl()._cursorPos.x;
	}

	PXE_NODISCARD int32_t PxeInputManager::getCursorYPos() const
	{
		return impl()._cursorPos.y;
	}

	PXE_NODISCARD int32_t PxeInputManager::getCursorXChange() const
	{
		return impl()._cursorChange.x;
	}

	PXE_NODISCARD int32_t PxeInputManager::getCursorYChange() const
	{
		return impl()._cursorChange.y;
	}

	PXE_NODISCARD const glm::i32vec2& PxeInputManager::getCursorPos() const
	{
		return impl()._cursorPos;
	}

	PXE_NODISCARD const glm::i32vec2& PxeInputManager::getCursorChange() const
	{
		return impl()._cursorChange;
	}

	PXE_NODISCARD PxeWindow* PxeInputManager::getMouseFocusedWindow() const
	{
		return impl()._focusedMouseWindow;
	}

	PXE_NODISCARD PxeWindow* PxeInputManager::getKeyboardFocusedWindow() const
	{
		return impl()._focusedKeyboardWindow;
	}

	void PxeInputManager::clearLastActionSource()
	{
		impl()._lastActionCode = 0;
	}

	void PxeInputManager::processEvents()
	{
		Impl& imp = impl();
		PxeEngine& engine = PxeEngine::getInstance();
		const std::unordered_map<uint32_t, PxeWindow*>& windows = engine.acquireWindows();
		imp._cursorChange = glm::i32vec2(0);
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_QUIT:
			{
				for (auto winPair : windows)
					winPair.second->setShouldClose();
			}
			break;

			case SDL_WINDOWEVENT:
			{
				auto windowItr = windows.find(evt.window.windowID);
				PxeWindow* window = nullptr;
				if (windowItr != windows.end() && windowItr->second->getAssetStatus() == PxeGLAssetStatus::INITIALIZED) window = windowItr->second;
				switch (evt.window.event)
				{
				case SDL_WINDOWEVENT_ENTER:
					imp._focusedMouseWindow = window;
					break;

				case SDL_WINDOWEVENT_LEAVE:
					if (imp._focusedMouseWindow == window)
						imp._focusedMouseWindow = nullptr;
					break;

				case SDL_WINDOWEVENT_FOCUS_GAINED:
					imp._focusedKeyboardWindow = window;
					break;

				case SDL_WINDOWEVENT_FOCUS_LOST:
					if (imp._focusedKeyboardWindow == window)
						imp._focusedKeyboardWindow = nullptr;
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
				if (imp._focusedMouseWindow) {
					imp._focusedMouseWindow->bindGuiContext();
					ImGui_ImplSDL2_ProcessEvent(&evt);
					if (!ImGui::GetIO().WantCaptureMouse) {
						glm::i32vec2 mousePos(evt.motion.x, evt.motion.y);
						imp._cursorChange = glm::i32vec2(evt.motion.xrel, evt.motion.yrel);
						imp._cursorPos = mousePos;
					}
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (imp._focusedMouseWindow) {
					imp._focusedMouseWindow->bindGuiContext();
					ImGui_ImplSDL2_ProcessEvent(&evt);
					if (!ImGui::GetIO().WantCaptureMouse) {
						PxeActionSourceCode code = PxeMouseActionSourceCode(evt.button.button);
						if (evt.button.state)
							imp._lastActionCode = code;
						auto sourceItr = imp._sources.find(code);
						if (sourceItr != imp._sources.end())
							sourceItr->second->setValue(evt.button.state);
					}
				}
				break;

			case SDL_MOUSEWHEEL:
				if (imp._focusedMouseWindow) {
					imp._focusedMouseWindow->bindGuiContext();
					ImGui_ImplSDL2_ProcessEvent(&evt);
				}
				break;

			case SDL_TEXTINPUT:
				if (imp._focusedKeyboardWindow) {
					imp._focusedKeyboardWindow->bindGuiContext();
					ImGui_ImplSDL2_ProcessEvent(&evt);
				}
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if (imp._focusedKeyboardWindow) {
					imp._focusedKeyboardWindow->bindGuiContext();
					ImGui_ImplSDL2_ProcessEvent(&evt);
					if (!ImGui::GetIO().WantCaptureKeyboard) {
						PxeActionSourceCode code = PxeKeyboardActionSourceCode(evt.key.keysym.sym);
						if (evt.key.type == SDL_KEYDOWN)
							imp._lastActionCode = code;
						auto sourceItr = imp._sources.find(code);
						if (sourceItr != imp._sources.end())
							sourceItr->second->setValue(evt.key.type == SDL_KEYDOWN);
					}
				}
				break;

			case SDL_JOYBUTTONUP:
			case SDL_JOYBUTTONDOWN:
			{
				PxeActionSourceCode code = PxeJoystickButtonActionSourceCode(evt.jbutton.button);
				if (evt.jbutton.state)
					imp._lastActionCode = code;
				auto sourceItr = imp._sources.find(code);
				if (sourceItr != imp._sources.end())
					sourceItr->second->setValue(evt.jbutton.state);
			}
			break;

			case SDL_JOYHATMOTION:
			{
				for (uint8_t hatDir = 1; hatDir < 0x10; hatDir = hatDir << 1) {
					PxeActionSourceCode code = PxeJoystickHatActionSourceCode(evt.jhat.hat, hatDir);
					if (evt.jhat.value & hatDir)
						imp._lastActionCode = code;
					auto sourceItr = imp._sources.find(code);
					if (sourceItr != imp._sources.end())
						sourceItr->second->setValue(evt.jhat.value & hatDir);
				}
			}
			break;

			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERBUTTONDOWN:
			{
				PxeActionSourceCode code = PxeControllerButtonActionSourceCode(evt.cbutton.button);
				if (evt.cbutton.state)
					imp._lastActionCode = code;
				auto sourceItr = imp._sources.find(code);
				if (sourceItr != imp._sources.end())
					sourceItr->second->setValue(evt.cbutton.state);
			}
			break;
			}
		}

		engine.releaseWindows();
	}

	void PxeInputManager::clearActiveWindow(PxeWindow* window)
	{
		Impl& imp = impl();
		if (window) {
			if (imp._focusedKeyboardWindow == window)
				imp._focusedKeyboardWindow = nullptr;
			if (imp._focusedMouseWindow == window)
				imp._focusedMouseWindow = nullptr;
		}
		else {
			imp._focusedMouseWindow = nullptr;
			imp._focusedKeyboardWindow = nullptr;
		}
	}
}