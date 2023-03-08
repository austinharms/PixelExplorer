#include "PxeGuiRenderProperties.h"

#include <new>

#include "PxeLogger.h"
#include "PxeEngine.h"
#include "PxeGuiShader.h"

namespace pxengine {
	PxeGuiRenderProperties* PxeGuiRenderProperties::s_propertiesInstance = nullptr;
	std::mutex PxeGuiRenderProperties::s_instanceMutex;

	PxeGuiRenderProperties* PxeGuiRenderProperties::getInstance()
	{
		std::lock_guard lock(s_instanceMutex);
		if (!s_propertiesInstance) {
			s_propertiesInstance = new(std::nothrow) PxeGuiRenderProperties();
			if (!s_propertiesInstance) {
				PXE_ERROR("Failed to allocate PxeGuiRenderProperties");
				return nullptr;
			}

			s_propertiesInstance->setShader(PxeEngine::getInstance().getRenderPipeline().getShader<PxeGuiShader>());
			if (!s_propertiesInstance->getShader()) {
				PXE_ERROR("Failed to load PxeGuiRenderProperties PxeGuiShader");
			}

			return s_propertiesInstance;
		}
		else {
			s_propertiesInstance->grab();
			return s_propertiesInstance;
		}
	}

	PxeGuiRenderProperties::PxeGuiRenderProperties() {
	
	}

	PxeGuiRenderProperties::~PxeGuiRenderProperties()
	{
	}

	void PxeGuiRenderProperties::onDelete()
	{
		std::lock_guard lock(s_instanceMutex);
		if (getRefCount()) return;
		s_propertiesInstance = nullptr;
	}
}
