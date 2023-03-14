#include "PxeShader.h"

#include "PxeEngine.h"
#include "PxeRenderPipeline.h"

namespace pxengine {
	std::mutex PxeShader::s_shaderIdMutex;
	uint32_t PxeShader::s_shaderIdCounter = 0;

	PxeShader::PxeShader() : PxeGLAsset(true) {
	
	}

	void PxeShader::onDelete()
	{
		if (getAssetStatus() >= PxeGLAssetStatus::INITIALIZED)
			PxeEngine::getInstance().getRenderPipeline().removeShader(*this);
		PxeGLAsset::onDelete();
	}

	PxeShaderId PxeShader::getNextShaderId()
	{
		std::lock_guard<std::mutex> lock(s_shaderIdMutex);
		return ++s_shaderIdCounter;
	}

	PxeShader::~PxeShader() {
	
	}
}