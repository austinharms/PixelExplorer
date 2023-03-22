#include "PxeGuiRenderProperties.h"

#include <new>

#include "PxeLogger.h"
#include "PxeEngine.h"
#include "PxeGuiShader.h"

namespace pxengine {
	PxeGuiRenderProperties* PxeGuiRenderProperties::create()
	{
		PxeGuiShader* guiShader = PxeEngine::getInstance().getRenderPipeline().loadShader<PxeGuiShader>();
		if (!guiShader) {
			PXE_ERROR("Failed to load PxeGuiRenderProperties PxeGuiShader");
			return nullptr;
		}

		PxeGuiRenderProperties* renderProps = new(std::nothrow) PxeGuiRenderProperties(*guiShader);
		guiShader->drop();
		if (!renderProps) {
			PXE_ERROR("Failed to allocate PxeGuiRenderProperties");
			return nullptr;
		}

		return renderProps;
	}

	PxeGuiRenderProperties::PxeGuiRenderProperties(PxeShader& guiShader) : _guiShader(guiShader) {
		guiShader.grab();
	}

	PXE_NODISCARD PxeShader* PxeGuiRenderProperties::getShader() const
	{
		return &_guiShader;
	}

	PxeGuiRenderProperties::~PxeGuiRenderProperties()
	{
		_guiShader.drop();
	}

	void PxeGuiRenderProperties::onApplyProperties()
	{
	}
}
