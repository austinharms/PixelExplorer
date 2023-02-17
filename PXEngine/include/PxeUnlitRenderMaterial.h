#ifndef PXENGINE_UNLIT_RENDER_MATERIAL_H_
#define PXENGINE_UNLIT_RENDER_MATERIAL_H_
#include "PxeTypes.h"
#include "PxeRenderMaterialInterface.h"
#include "PxeShader.h"

namespace pxengine {
	class PxeUnlitRenderMaterial : public PxeRenderMaterialInterface
	{
	public:
		// Creates and returns a new instance of PxeUnlitRenderMaterial, or nullptr on failure
		static PxeUnlitRenderMaterial* createMaterial();
		virtual ~PxeUnlitRenderMaterial();

		// Sets the tint color
		// Note: default value is <1,1,1> (white)
		void setColor(const glm::vec3& color);

	private:
		PxeUnlitRenderMaterial(PxeShader& shader);
	};
}
#endif // !PXENGINE_UNLIT_RENDER_MATERIAL_H_
