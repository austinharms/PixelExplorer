#ifndef PIXELEXPLORER_TERRAIN_RENDER_COMPONENT_H_
#define PIXELEXPLORER_TERRAIN_RENDER_COMPONENT_H_
#include "PxeTypes.h"
#include "PxeRenderComponent.h"
#include "TerrainRenderProperties.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBuffer.h"
#include "PxeVertexArray.h"

namespace pixelexplorer {
	namespace terrain {
		class TerrainRenderComponent : public pxengine::PxeRenderComponent
		{
		public:
			static TerrainRenderComponent* create(TerrainRenderProperties& renderProperties);
			// Note: this won't actually release the current renderMesh memory, but disable rendering until a new mesh is supplied
			void clearRenderMesh();
			void setRenderMesh(pxengine::PxeBuffer& indexBuffer, pxengine::PxeBuffer& vertexBuffer);
			PXE_DEFAULT_PUBLIC_COMPONENT_IMPLMENTATION(TerrainRenderComponent, pxengine::PxeRenderComponent);
			virtual ~TerrainRenderComponent();
			PXE_NOCOPY(TerrainRenderComponent);

		protected:
			TerrainRenderComponent(TerrainRenderProperties& renderProperties, pxengine::PxeVertexArray& vertexArray, pxengine::PxeVertexBuffer& vertexBuffer, pxengine::PxeIndexBuffer& indexBuffer);
			PXE_DEFAULT_PROTECTED_COMPONENT_IMPLMENTATION(pxengine::PxeRenderComponent);
			void onRender() override;

		private:
			pxengine::PxeVertexArray& _vertexArray;
			pxengine::PxeVertexBuffer& _vertexBuffer;
			pxengine::PxeIndexBuffer& _indexBuffer;
			bool _cleared;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_RENDER_COMPONENT_H_
