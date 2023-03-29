#include "TerrainRenderComponent.h"

#include <new>

#include "Log.h"
#include "GL/glew.h"

namespace pixelexplorer {
	namespace terrain {
		TerrainRenderComponent* TerrainRenderComponent::create(TerrainRenderProperties& renderProperties)
		{
			using namespace pxengine;
			PxeVertexBufferFormat fmt;
			if (!fmt.getAttribCount()) {
				fmt.addAttrib(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
				fmt.addAttrib(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
				fmt.addAttrib(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
			}

			PxeVertexArray* vertArray = new(std::nothrow) PxeVertexArray(true);
			if (!vertArray) { 
				PEX_ERROR("Failed to allocate TerrainRenderComponent's PxeVertexArray");
				return nullptr;
			}

			PxeIndexBuffer* indexBuf = new(std::nothrow) PxeIndexBuffer(PxeIndexType::UNSIGNED_16BIT, nullptr, true);
			if (!indexBuf) {
				PEX_ERROR("Failed to allocate TerrainRenderComponent's PxeIndexBuffer");
				vertArray->drop();
				return nullptr;
			}

			PxeVertexBuffer* vertBuf = new(std::nothrow) PxeVertexBuffer(fmt, nullptr, true);
			if (!vertBuf) {
				PEX_ERROR("Failed to allocate TerrainRenderComponent's PxeVertexBuffer");
				vertArray->drop();
				indexBuf->drop();
				return nullptr;
			}

			vertArray->setIndexBuffer(indexBuf);
			vertArray->addVertexBuffer(*vertBuf, 0, 0);
			vertArray->addVertexBuffer(*vertBuf, 1, 1);
			vertArray->addVertexBuffer(*vertBuf, 2, 2);

			TerrainRenderComponent* cmp = new(std::nothrow) TerrainRenderComponent(renderProperties, *vertArray, *vertBuf, *indexBuf);
			if (!cmp) { 
				PEX_ERROR("Failed to allocate TerrainRenderComponent");
				vertArray->drop();
				indexBuf->drop();
				vertBuf->drop();
				return nullptr;
			}

			indexBuf->initializeAsset();
			vertBuf->initializeAsset();
			vertArray->initializeAsset();
			return cmp;
		}

		void TerrainRenderComponent::clearRenderMesh()
		{
			_cleared = true;
		}

		void TerrainRenderComponent::setRenderMesh(pxengine::PxeBuffer& indexBuffer, pxengine::PxeBuffer& vertexBuffer)
		{
			_indexBuffer.bufferData(indexBuffer);
			_vertexBuffer.bufferData(vertexBuffer);
			_cleared = false;
		}

		TerrainRenderComponent::TerrainRenderComponent(TerrainRenderProperties& renderProperties, pxengine::PxeVertexArray& vertexArray, pxengine::PxeVertexBuffer& vertexBuffer, pxengine::PxeIndexBuffer& indexBuffer) : 
			pxengine::PxeRenderComponent(renderProperties), _vertexArray(vertexArray), _vertexBuffer(vertexBuffer), _indexBuffer(indexBuffer) { _cleared = true; }

		TerrainRenderComponent::~TerrainRenderComponent() {
			_vertexArray.drop();
			_vertexBuffer.drop();
			_indexBuffer.drop();
		}

		void TerrainRenderComponent::onRender()
		{
			if (_cleared) return;
			_vertexArray.bind();
			glDrawElements(GL_TRIANGLES, _indexBuffer.getIndexCount(), (uint32_t)_indexBuffer.getIndexType(), nullptr);
   			_vertexArray.unbind();
		}
	}
}