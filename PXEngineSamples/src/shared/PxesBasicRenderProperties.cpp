#include "PxesBasicRenderProperties.h"

#include <cassert>

#include "PxeEngine.h"

PxesBasicRenderProperties::PxesBasicRenderProperties(pxengine::PxeTexture& texture) : _texture(texture)
{
	using namespace pxengine;
	_texture.grab();
	PxeEngine& engine = PxeEngine::getInstance();
	assert((_basicShader = engine.getRenderPipeline().loadShader<PxesBasicShader>()));
}

PxesBasicRenderProperties::~PxesBasicRenderProperties()
{
	_texture.drop();
	_basicShader->drop();
}

PXE_NODISCARD pxengine::PxeShader* PxesBasicRenderProperties::getShader() const
{
	return static_cast<pxengine::PxeShader*>(_basicShader);
}

void PxesBasicRenderProperties::onApplyProperties()
{
	_basicShader->setTexture(_texture.getGlTextureId());
}
