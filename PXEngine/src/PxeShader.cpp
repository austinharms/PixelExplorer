#include "PxeShader.h"

namespace pxengine {
	std::mutex PxeShader::s_shaderIdMutex;
	uint32_t PxeShader::s_shaderIdCounter = 0;

	PxeShader::PxeShader(bool delayAssetInitialization) : PxeGLAsset(delayAssetInitialization) {
	
	}

	PxeShaderId PxeShader::getNextShaderId()
	{
		std::lock_guard<std::mutex> lock(s_shaderIdMutex);
		return ++s_shaderIdCounter;
	}

	PxeShader::~PxeShader() {
	
	}
}