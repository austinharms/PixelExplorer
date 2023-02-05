#ifndef PXENGINESAMPELS_SCENES_FREE_CAMERA_RENDER_CUBE_H_
#define PXENGINESAMPELS_SCENES_FREE_CAMERA_RENDER_CUBE_H_
#include <new>

#include "PxeEngineAPI.h"
#include "GL/glew.h"
namespace freecamera {
	class RenderCube : public pxengine::PxeRenderObject
	{
	public:
		RenderCube(pxengine::PxeRenderMaterial& material, pxengine::PxeIndexBuffer* indexBuffer = nullptr, pxengine::PxeVertexArray* vertextArray = nullptr) : pxengine::PxeRenderObject(material) {

			if (indexBuffer) {
				_indexBuffer = indexBuffer;
				_indexBuffer->grab();
			}
			else {
				_indexBuffer = new(std::nothrow) pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_8BIT);
				uint8_t indices[36] = {
					0, 2, 1, 0, 3, 2,  // Front
					4, 5, 6, 4, 6, 7,  // Back
					0, 4, 7, 0, 7, 3,  // Left
					1, 6, 5, 1, 2, 6,  // Right
					3, 7, 6, 3, 6, 2,  // Top
					0, 5, 4, 0, 1, 5,  // Bottom
				};

				pxengine::PxeBuffer* indexData = new(std::nothrow) pxengine::PxeBuffer(sizeof(indices));
				memcpy(indexData->getBuffer(), indices, indexData->getSize());
				_indexBuffer->bufferData(*indexData);
				indexData->drop();
				indexData = nullptr;
			}

			if (vertextArray) {
				_vertexArray = vertextArray;
				_vertexArray->grab();
			}
			else {
				pxengine::PxeVertexBufferFormat format(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
				format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 2, false));
				pxengine::PxeVertexBuffer* vertexBuffer = new(std::nothrow) pxengine::PxeVertexBuffer(format);
				_vertexArray = new(std::nothrow) pxengine::PxeVertexArray();
				_vertexArray->addVertexBuffer(*vertexBuffer, 0, 0);
				_vertexArray->addVertexBuffer(*vertexBuffer, 1, 1);

				float vertices[40] = {
				 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  // 0
				  0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  // 1
				  0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  // 2
				 -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  // 3
				 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  // 4
				  0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  // 5
				  0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  // 6
				 -0.5f,  0.5f,  0.5f,  1.0f,  0.0f   // 7
				};

				pxengine::PxeBuffer* vertexData = new(std::nothrow) pxengine::PxeBuffer(sizeof(vertices));
				memcpy(vertexData->getBuffer(), vertices, vertexData->getSize());
				vertexBuffer->bufferData(*vertexData);
				vertexData->drop();
				vertexData = nullptr;
				vertexBuffer->drop();
				vertexBuffer = nullptr;
			}
		}

		virtual ~RenderCube() {
			_indexBuffer->drop();
			_vertexArray->drop();
		}

		void onGeometry() override {
			_vertexArray->bind();
			_indexBuffer->bind();
			glDrawElements(GL_TRIANGLES, _indexBuffer->getIndexCount(), (uint32_t)_indexBuffer->getIndexType(), nullptr);
			_indexBuffer->unbind();
			_vertexArray->unbind();
		}

		void translate(const glm::vec3& pos) {
			positionMatrix = glm::translate(positionMatrix, pos);
		}

		void rotateAbout(const glm::vec3& axis, float rad) {
			positionMatrix = glm::rotate(positionMatrix, rad, axis);
		}

		pxengine::PxeIndexBuffer* getIndexBuffer() const { return _indexBuffer; }

		pxengine::PxeVertexArray* getVertexArray() const { return _vertexArray; }

	private:
		pxengine::PxeIndexBuffer* _indexBuffer;
		pxengine::PxeVertexArray* _vertexArray;
	};
}
#endif // !PXENGINESAMPELS_SCENES_FREE_CAMERA_RENDER_CUBE_H_