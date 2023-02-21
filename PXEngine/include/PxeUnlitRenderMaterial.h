#ifndef PXENGINE_UNLIT_RENDER_MATERIAL_H_
#define PXENGINE_UNLIT_RENDER_MATERIAL_H_
#include "PxeTypes.h"
#include "PxeRenderMaterialInterface.h"
#include "PxeShader.h"
#include "PxeTexture.h"

namespace pxengine {
	class PxeUnlitRenderMaterial : public PxeRenderMaterialInterface
	{
	public:
		// Creates and returns a new instance of PxeUnlitRenderMaterial, or nullptr on failure
		static PxeUnlitRenderMaterial* createMaterial();
		virtual ~PxeUnlitRenderMaterial();

		// Sets the tint color
		// Note: default value is <1,1,1,1> (white)
		void setColor(const glm::vec4& color);
		// Sets the active texture or pass nullptr to disable the texture
		// Note: this sets the active OpenGl texture slot to GL_TEXTURE0
		void setTexture(PxeTexture* texture);

		void applyMaterial() override;

		PxeShader& getShader() const override;

	private:
		enum UNIFORMS
		{
			COLOR = 0,
			TEXTURE,
			UNIFORM_COUNT
		};

		PxeUnlitRenderMaterial(PxeShader& shader);
		void loadLocations();

		PxeShader& _shader;
		int32_t _locations[UNIFORM_COUNT];
		glm::vec4 _color;
		PxeTexture* _texture;
		bool _loadedLocations;
	};
}
#endif // !PXENGINE_UNLIT_RENDER_MATERIAL_H_
