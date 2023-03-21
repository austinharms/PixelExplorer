#ifndef PIXELEXPLORER_TOOLS_RENDER_LINE_H_
#define PIXELEXPLORER_TOOLS_RENDER_LINE_H_
#include <mutex>
#include <new>
#include <stdint.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "PxeRenderObject.h"
#include "PxeVertexArray.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBuffer.h"
#include "PxeBuffer.h"
#include "PxeUnlitRenderMaterial.h"

namespace pixelexplorer {
	namespace tools {
		class RenderLine : public pxengine::PxeRenderObject
		{
		public:
			static RenderLine* createLine();
			void onRender() override;
			virtual ~RenderLine();

			glm::vec3 PointA;
			glm::vec3 PointB;
			glm::vec4 Color;
			float Width;
			bool Disabled;

		private:
			static std::mutex s_renderDataMutex;
			static pxengine::PxeUnlitRenderMaterial* s_material;
			static pxengine::PxeVertexArray* s_vertexArray;
			static pxengine::PxeVertexBuffer* s_vertexBuffer;
			static pxengine::PxeIndexBuffer* s_indexBuffer;
			static pxengine::PxeBuffer* s_vertexData;

			RenderLine(pxengine::PxeRenderMaterialInterface& material);
		};
	}
}
#endif // !PIXELEXPLORER_TOOLS_RENDER_LINE_H_
