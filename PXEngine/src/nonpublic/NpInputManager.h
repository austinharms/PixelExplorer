#ifndef PXENGINE_NONPUBLIC_ACTION_MANAGER_H_
#define PXENGINE_NONPUBLIC_ACTION_MANAGER_H_
#include <string>
#include <unordered_map>

#include "PxeTypes.h"
#include "PxeInputManager.h"
#include "PxeAction.h"
#include "PxeActionSource.h"
#include "NpWindow.h"

namespace pxengine {
	namespace nonpublic {
		class NpInputManager : public PxeInputManager
		{
		public:
			//############# PxeInputManager API ##################

			PXE_NODISCARD PxeActionSource* getLastActionSource() override;
			PXE_NODISCARD PxeActionSource* getActionSource(PxeActionSourceCode sourceCode) override;
			PXE_NODISCARD PxeAction* getAction(const std::string& actionName) override;
			PXE_NODISCARD int32_t getCursorXPos() const override;
			PXE_NODISCARD int32_t getCursorYPos() const override;
			PXE_NODISCARD int32_t getCursorXChange() const override;
			PXE_NODISCARD int32_t getCursorYChange() const override;
			PXE_NODISCARD const glm::i32vec2& getCursorPos() const override;
			PXE_NODISCARD const glm::i32vec2& getCursorChange() const override;
			PXE_NODISCARD PxeWindow* getMouseFocusedWindow() const override;
			PXE_NODISCARD PxeWindow* getKeyboardFocusedWindow() const override;

			//############# PRIVATE API ##################

			NpInputManager();
			virtual ~NpInputManager();
			void processEvents();
			void clearActiveWindow(NpWindow* window = nullptr);

		private:
			NpWindow* _focusedKeyboardWindow;
			NpWindow* _focusedMouseWindow;
			PxeActionSourceCode _lastActionCode;
			std::unordered_map<PxeActionSourceCode, PxeActionSource*> _sources;
			std::unordered_map<std::string, PxeAction*> _actions;
			glm::i32vec2 _cursorPos;
			glm::i32vec2 _cursorChange;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_ACTION_MANAGER_H_
