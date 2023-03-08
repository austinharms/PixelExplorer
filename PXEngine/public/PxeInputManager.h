#ifndef PXENGINE_INPUT_MANAGER_H_
#define PXENGINE_INPUT_MANAGER_H_
#include <string>

#include "PxeTypes.h"
#include "PxeActionSource.h"
#include "PxeAction.h"
#include "PxeWindow.h"

namespace pxengine {
	class PxeEngine;

	// TODO Comment file
	class PxeInputManager
	{
	public:
		// Returns the last PxeActionSource that was activated
		PXE_NODISCARD PxeActionSource* getLastActionSource();
		PXE_NODISCARD PxeActionSource* getActionSource(PxeActionSourceCode sourceCode);
		PXE_NODISCARD PxeAction* getAction(const std::string& actionName);
		PXE_NODISCARD int32_t getCursorXPos() const;
		PXE_NODISCARD int32_t getCursorYPos() const;
		PXE_NODISCARD int32_t getCursorXChange() const;
		PXE_NODISCARD int32_t getCursorYChange() const;
		PXE_NODISCARD const glm::i32vec2& getCursorPos() const;
		PXE_NODISCARD const glm::i32vec2& getCursorChange() const;
		PXE_NODISCARD PxeWindow* getMouseFocusedWindow() const;
		PXE_NODISCARD PxeWindow* getKeyboardFocusedWindow() const;
		void clearLastActionSource();

		virtual ~PxeInputManager();
		PXE_NOCOPY(PxeInputManager);

	private:
		PxeInputManager();
		static constexpr PxeSize STORAGE_SIZE = 200;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	public:
		void processEvents();
		void clearActiveWindow(PxeWindow* window = nullptr);

	private:
		friend class PxeEngine;
		struct Impl;

		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_INPUT_MANAGER_H_
