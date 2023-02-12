#ifndef PIXELEXPLORER_TOOLS_RENDER_POINT_H_
#define PIXELEXPLORER_TOOLS_RENDER_POINT_H_
#include <mutex>
#include <new>
#include <stdint.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "PxeRenderObject.h"
#include "PxeRenderMaterial.h"
#include "PxeVertexArray.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBuffer.h"
#include "PxeBuffer.h"

namespace pixelexplorer {
	namespace tools {
		class RenderPoint : public pxengine::PxeRenderObject
		{
		public:
			static RenderPoint* createPoint();
			void onGeometry() override;
			virtual ~RenderPoint();

			glm::vec3 Point;
			glm::vec4 Color;
			float Width;
			bool Disabled;

		private:
			static std::mutex s_renderDataMutex;
			static pxengine::PxeRenderMaterial* s_material;
			static pxengine::PxeVertexArray* s_vertexArray;

			RenderPoint(pxengine::PxeRenderMaterial& material);
		};
	}
}
#endif // !PIXELEXPLORER_TOOLS_RENDER_POINT_H_
