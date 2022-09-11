#include <string>
#include <stdint.h>

#include "common/RefCount.h"
#include "InputSource.h"

#ifndef PIXELEXPLORER_ENGINE_INPUT_INPUTACTION_H_
#define PIXELEXPLORER_ENGINE_INPUT_INPUTACTION_H_
namespace pixelexplorer::engine::input {
	class InputAction : public RefCount
	{
	public:
		inline virtual ~InputAction() {}

		inline const std::string& getName() const { return _name; }

		inline const double getValue() const { return _value; }

		inline const double getTrueValue(double deltaTime) const {
			if (_source.getIsPrecise()) deltaTime = 1;
			return _value * deltaTime;
		}

		inline const InputSource& getSource() const { return _source; }

	private:
		friend class InputManager;
		const std::string _name;
		InputSource _source;
		double _value;

		inline InputAction(const std::string& name, const InputSource& source) : _name(name), _value(0), _source(source) {}

		inline void setValue(double value) {
			if (_source.ActionInverted)
				value = -value;
			if (!_source.AllowNegativeValues && value < 0)
				value = 0;
			_value = value;
		}
	};
}
#endif // !PIXELEXPLORER_ENGINE_INPUTACTION_H_
