#ifndef PXENGINE_NONPUBLIC_GUI_RENDER_MATERIAL_H_
#define PXENGINE_NONPUBLIC_GUI_RENDER_MATERIAL_H_
#include "PxeTypes.h"
#include "PxeShader.h"
#include "PxeRenderMaterialInterface.h"

namespace pxengine {
	namespace nonpublic {
		class NpGuiRenderMaterial : public PxeRenderMaterialInterface {
		public:
			NpGuiRenderMaterial(PxeShader& shader);
			virtual ~NpGuiRenderMaterial();
			void applyMaterial() override;
			PXE_NODISCARD PxeShader& getShader() const override;
			void setProjectionMatrix(const glm::mat4& mat);
			void setTexture(uint32_t texture);

		private:
			PxeShader& _guiShader;
			int32_t _projLocation;
			int32_t _textureLocation;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_GUI_RENDER_MATERIAL_H_
