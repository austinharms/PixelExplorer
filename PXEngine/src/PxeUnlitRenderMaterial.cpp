#include "PxeUnlitRenderMaterial.h"

#include <new>

#include "PxeFSHelpers.h"
#include "NpEngine.h"
#include "NpLogger.h"

namespace pxengine {
	PxeUnlitRenderMaterial* PxeUnlitRenderMaterial::createMaterial()
	{
		PxeShader* shader = nonpublic::NpEngine::getInstance().loadShader(getAssetPath("shaders") / "pxengine_unlit_shader.pxeshader");
		if (!shader) {
			PXE_ERROR("Failed to load unlit PxeShader");
			return nullptr;
		}

		PxeUnlitRenderMaterial* material = new(std::nothrow) PxeUnlitRenderMaterial(*shader);
		if (!material) { PXE_ERROR("Failed to allocate PxeUnlitRenderMaterial"); }
		return material;
	}

	PxeUnlitRenderMaterial::PxeUnlitRenderMaterial(PxeShader& shader) : PxeRenderMaterial(shader, PxeRenderPass::FORWARD) { }

	PxeUnlitRenderMaterial::~PxeUnlitRenderMaterial() { }
}