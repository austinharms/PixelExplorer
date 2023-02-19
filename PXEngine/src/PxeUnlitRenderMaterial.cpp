#include "PxeUnlitRenderMaterial.h"

#include <new>

#include "NpLogger.h"
#include "PxeFSHelpers.h"
#include "NpEngine.h"
#include "PxeRenderPipeline.h"
#include "GL/glew.h"

namespace pxengine {
	PxeUnlitRenderMaterial* PxeUnlitRenderMaterial::createMaterial()
	{
		PxeShader* shader = nonpublic::NpEngine::getInstance().getRenderPipeline().loadShader(getAssetPath("shaders") / "pxengine_unlit_shader.pxeshader");
		if (!shader) {
			PXE_ERROR("Failed to load unlit PxeShader");
			return nullptr;
		}

		PxeUnlitRenderMaterial* material = new(std::nothrow) PxeUnlitRenderMaterial(*shader);
		shader->drop();
		if (!material) { 
			PXE_ERROR("Failed to allocate PxeUnlitRenderMaterial");
			return nullptr;
		}

		return material;
	}

	void PxeUnlitRenderMaterial::setColor(const glm::vec3& color)
	{
		_color = color;
	}

	void PxeUnlitRenderMaterial::applyMaterial()
	{
		if (!_loadedLocations) loadLocations();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_shader.setUniform3f(_locations[COLOR], _color);
	}

	PxeShader& PxeUnlitRenderMaterial::getShader() const
	{
		return _shader;
	}

	PxeUnlitRenderMaterial::PxeUnlitRenderMaterial(PxeShader& shader) : PxeRenderMaterialInterface(PxeRenderPass::FORWARD), _shader(shader) {
		_shader.grab();
		_color = glm::vec3(1);
		_loadedLocations = false;
	}

	void PxeUnlitRenderMaterial::loadLocations()
	{
		constexpr const char* uniformNames[] = {
			"u_Color"
		};

		for (int32_t i = 0; i < UNIFORM_COUNT; ++i)
			_locations[i] = _shader.getUniformLocation(uniformNames[i]);
		_loadedLocations = true;
	}

	PxeUnlitRenderMaterial::~PxeUnlitRenderMaterial() { 
		_shader.drop();
	}
}