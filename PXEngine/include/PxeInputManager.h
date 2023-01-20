#ifndef PXENGINE_INPUT_MANAGER_H_
#define PXENGINE_INPUT_MANAGER_H_
#include <string>

#include "PxeTypes.h"
#include "PxeAction.h"
#include "PxeActionSource.h"
#include "PxeWindow.h"

namespace pxengine {
	// TODO Comment file
	class PxeInputManager
	{
	public:
		PxeInputManager() = default;
		virtual ~PxeInputManager() = default;

		// Returns the last PxeActionSource that was activated
		virtual PXE_NODISCARD PxeActionSource* getLastActionSource() = 0;
		virtual PXE_NODISCARD PxeActionSource* getActionSource(PxeActionSourceCode sourceCode) = 0;
		virtual PXE_NODISCARD PxeAction* getAction(const std::string& actionName) = 0;
		virtual PXE_NODISCARD int32_t getCursorXPos() const = 0;
		virtual PXE_NODISCARD int32_t getCursorYPos() const = 0;
		virtual PXE_NODISCARD int32_t getCursorXChange() const = 0;
		virtual PXE_NODISCARD int32_t getCursorYChange() const = 0;
		virtual PXE_NODISCARD const glm::i32vec2& getCursorPos() const = 0;
		virtual PXE_NODISCARD const glm::i32vec2& getCursorChange() const = 0;
		virtual PXE_NODISCARD PxeWindow* getMouseFocusedWindow() const = 0;
		virtual PXE_NODISCARD PxeWindow* getKeyboardFocusedWindow() const = 0;
		virtual void clearLastActionSource() = 0;
	};
}
#endif // !PXENGINE_INPUT_MANAGER_H_
