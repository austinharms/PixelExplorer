#include "PxeRenderProperties.h"

namespace pxengine {
	PxeRenderProperties::~PxeRenderProperties()
	{
		setShader(nullptr);
	}

	PxeRenderProperties::PxeRenderProperties(PxeShader* shader)
	{
		_renderShader = nullptr;
		setShader(shader);
	}

	void PxeRenderProperties::setShader(PxeShader* shader)
	{
		if (_renderShader != nullptr) _renderShader->drop();
		if ((_renderShader = shader)) _renderShader->grab();
	}

	PXE_NODISCARD PxeShader* PxeRenderProperties::getShader() const
	{
		return _renderShader;
	}
}