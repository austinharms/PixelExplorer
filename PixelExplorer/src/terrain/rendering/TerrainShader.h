#ifndef PIXELEXPLORER_TERRAIN_SHADER_H_
#define PIXELEXPLORER_TERRAIN_SHADER_H_
#include "PxeGlShaderProgram.h"
#include "PxeShader.h"
namespace pixelexplorer {
	namespace terrain {
		class TerrainShader : public pxengine::PxeShader
		{
		public:
			TerrainShader();
			virtual ~TerrainShader();
			PXE_NODISCARD pxengine::PxeShaderExecutionOrder getExecutionOrder() const override;
			PXE_NODISCARD pxengine::PxeShaderId getShaderId() override;
			void setLightDirection(const glm::vec3& dir);
			void setLightColor(const glm::vec3& color);

		protected:
			void initializeGl() override;
			void uninitializeGl() override;
			void bind() override;
			void unbind() override;
			void setRenderTarget(pxengine::PxeRenderTarget& renderTarget) override;
			void setObjectTransform(const glm::mat4& transform) override;

		private:
			pxengine::PxeGlShaderProgram* _shaderProgram;
			int32_t _uMVPLocation;
			int32_t _uCameraLocation;
			int32_t _uLightDirectionLocation;
			int32_t _uLightColorLocation;

			glm::mat4 pvMatrix;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_SHADER_H_
