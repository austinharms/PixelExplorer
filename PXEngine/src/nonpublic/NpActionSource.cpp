#include "NpActionSource.h"

namespace pxengine {
	namespace nonpublic {
		PXE_NODISCARD float NpActionSource::getValue() const
		{
			return _value;
		}

		PXE_NODISCARD PxeActionSourceCode NpActionSource::getSourceCode() const
		{
			return _sourceCode;
		}

		NpActionSource::NpActionSource(PxeActionSourceCode code) : _sourceCode(code) {
			_value = 0;
		}

		void NpActionSource::setValue(float value)
		{
			_value = value;
		}
	}
}