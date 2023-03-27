#ifndef PIXELEXPLORER_TERRAIN_RENDER_PROPERTIES_H_
#define PIXELEXPLORER_TERRAIN_RENDER_PROPERTIES_H_
#include "PxeRenderProperties.h"
#include "PxeShader.h"
#include "TerrainShader.h"

namespace pixelexplorer {
	namespace terrain {
		class TerrainRenderProperties : public pxengine::PxeRenderProperties
		{
		public:
			TerrainRenderProperties();
			virtual ~TerrainRenderProperties();
			PXE_NODISCARD pxengine::PxeShader* getShader() const override;
			void setLightDirection(const glm::vec3& dir);
			void setLightColor(const glm::vec3& color);

		protected:
			void onApplyProperties() override;

		private:
			TerrainShader* _shader;
			glm::vec3 _dirLightDirection;
			glm::vec3 _dirLightColor;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_RENDER_PROPERTIES_H_
