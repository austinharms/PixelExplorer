#include "TerrainRenderProperties.h"

#include "Log.h"
#include "PxeEngine.h"

namespace pixelexplorer {
	namespace terrain {
		TerrainRenderProperties::TerrainRenderProperties()
		{
			using namespace pxengine;
			PxeEngine& engine = PxeEngine::getInstance();
			if (!(_shader = engine.getRenderPipeline().loadShader<TerrainShader>())) {
				PEX_FATAL("Failed to load TerrainShader");
			}
		
			_dirLightColor = glm::vec3(1);
			_dirLightDirection = glm::vec3(0, -1, 0);
		}

		TerrainRenderProperties::~TerrainRenderProperties()
		{
			_shader->drop();
		}

		PXE_NODISCARD pxengine::PxeShader* TerrainRenderProperties::getShader() const
		{
			return static_cast<pxengine::PxeShader*>(_shader);
		}

		void TerrainRenderProperties::setLightDirection(const glm::vec3& dir)
		{
			_dirLightDirection = dir;
		}

		void TerrainRenderProperties::setLightColor(const glm::vec3& color)
		{
			_dirLightColor = color;
		}

		void TerrainRenderProperties::onApplyProperties()
		{
			_shader->setLightDirection(_dirLightDirection);
			_shader->setLightColor(_dirLightColor);
		}
	}
}
