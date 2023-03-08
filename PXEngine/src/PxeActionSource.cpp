#include "PxeActionSource.h"

namespace pxengine {
	PxeActionSource::PxeActionSource(PxeActionSourceCode code) : _sourceCode(code) {
		_value = 0;
	}

	PxeActionSource::~PxeActionSource() {
	
	}

	PXE_NODISCARD float PxeActionSource::getValue() const
	{
		return _value;
	}

	PXE_NODISCARD PxeActionSourceCode PxeActionSource::getSourceCode() const
	{
		return _sourceCode;
	}

	void PxeActionSource::setValue(float value)
	{
		_value = value;
	}
}