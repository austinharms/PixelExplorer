#include <stdint.h>

#ifndef PIXELEXPLORER_ENGINE_INPUTSOURCE_H_
#define PIXELEXPLORER_ENGINE_INPUTSOURCE_H_

#define PX_X_AXIS 2
#define PX_Y_AXIS 3

namespace pixelexplorer::engine::input {
	struct InputSource
	{
	public:
		enum class InputSourceType : int16_t
		{
			UNKNOWN = -1,
			KEYBOARD,
			CURSORPOS,
			MOUSESCROLL,
			MOUSEBUTTON,
			JOYSTICKANALOG,
			JOYSTICKBUTTON,
			SOURCECOUNT
		};

		InputSourceType Source = InputSourceType::UNKNOWN;
		int32_t SourceAction = -1;
		bool ActionInverted = false;
		bool AllowNegativeValues = false;

		inline uint64_t getHash() const {
			return (((uint64_t)SourceAction) << 32) | (uint64_t)Source;
		}

		inline uint64_t getFullHash() const {
			return (((uint64_t)SourceAction) << 32) | (((uint64_t)Source) << 16) | (((uint64_t)ActionInverted) << 8) | ((uint64_t)AllowNegativeValues);
		}

		inline bool operator ==(const InputSource& other) const {
			return Source == other.Source && SourceAction == other.SourceAction && ActionInverted == other.ActionInverted && AllowNegativeValues == other.AllowNegativeValues;
		}

		inline bool operator <(const InputSource& other) const {
			return getHash() < other.getHash();
		}

		bool getIsPrecise() const {
			return Source == InputSourceType::CURSORPOS || Source == InputSourceType::MOUSESCROLL;
		}
	};
}
#endif // !PIXELEXPLORER_ENGINE_INPUTSOURCE_H_
