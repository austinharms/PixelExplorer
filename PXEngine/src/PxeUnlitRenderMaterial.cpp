#include "PxeUnlitRenderMaterial.h"

#include <new>
#include <cstring>

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

	void PxeUnlitRenderMaterial::setColor(const glm::vec4& color)
	{
		_color = color;
	}

	void PxeUnlitRenderMaterial::setTexture(PxeTexture* texture)
	{
		if (_texture)
			_texture->drop();
		_texture = texture;
		if (_texture) _texture->grab();
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
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_STENCIL_TEST);
		glActiveTexture(GL_TEXTURE0);
		_shader.setUniform1i(_locations[TEXTURE], 0);
		if (_texture && _texture->getAssetStatus() == PxeGLAssetStatus::INITIALIZED) {
			glBindTexture(GL_TEXTURE_2D, _texture->getGlTextureId());
		}
		else {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		_shader.setUniform4f(_locations[COLOR], _color);
	}

	PxeShader& PxeUnlitRenderMaterial::getShader() const
	{
		return _shader;
	}

	PxeUnlitRenderMaterial::PxeUnlitRenderMaterial(PxeShader& shader) : PxeRenderMaterialInterface(PxeRenderPass::FORWARD), _shader(shader) {
		_shader.grab();
		memset(_locations, 0, sizeof(_locations));
		_color = glm::vec4(1);
		_texture = nullptr;
		_loadedLocations = false;
	}

	void PxeUnlitRenderMaterial::loadLocations()
	{
		constexpr const char* uniformNames[] = {
			"u_Color",
			"u_Texture"
		};

		for (int32_t i = 0; i < UNIFORM_COUNT; ++i)
			_locations[i] = _shader.getUniformLocation(uniformNames[i]);
		_loadedLocations = true;
	}

	PxeUnlitRenderMaterial::~PxeUnlitRenderMaterial() { 
		_shader.drop();
		setTexture(nullptr);
	}
}