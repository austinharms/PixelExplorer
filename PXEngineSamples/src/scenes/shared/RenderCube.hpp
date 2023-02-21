#ifndef PXENGINESAMPELS_SCENES_SHARED_RENDER_CUBE_H_
#define PXENGINESAMPELS_SCENES_SHARED_RENDER_CUBE_H_
#include <new>
#include <mutex>

#include "PxeTypes.h"
#include "PxeRenderObject.h"
#include "PxeVertexArray.h"
#include "PxeVertexBuffer.h"
#include "PxeBuffer.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBufferFormat.h"
#include "PxeVertexBufferAttrib.h"
#include "PxeRenderMaterialInterface.h"
#include "PxeUnlitRenderMaterial.h"
#include "RenderData.h"
#include "GL/glew.h"

class RenderCube : public pxengine::PxeRenderObject
{
public:
	static RenderCube* create(pxengine::PxeRenderMaterialInterface& material) {
		std::lock_guard lock(s_mutex);
		bool createdVAO = false;
		if (!s_vertexArray) {
			// Create and load vertex buffer
			pxengine::PxeVertexBufferFormat format;
			format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
			format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 2, false));
			pxengine::PxeVertexBuffer* vertexBuffer = new pxengine::PxeVertexBuffer(format);
			pxengine::PxeBuffer* vertexData = new pxengine::PxeBuffer(sizeof(renderCubeVertices));
			memcpy(vertexData->getBuffer(), renderCubeVertices, vertexData->getSize());
			vertexBuffer->bufferData(*vertexData);
			vertexData->drop();

			// Create and load index buffer
			pxengine::PxeIndexBuffer* indexBuffer = new pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_8BIT);
			pxengine::PxeBuffer* indexData = new pxengine::PxeBuffer(sizeof(renderCubeIndices));
			memcpy(indexData->getBuffer(), renderCubeIndices, indexData->getSize());
			indexBuffer->bufferData(*indexData);
			indexData->drop();

			// Attach buffers to VAO
			s_vertexArray = new pxengine::PxeVertexArray();
			s_vertexArray->addVertexBuffer(*vertexBuffer, 0, 0);
			s_vertexArray->addVertexBuffer(*vertexBuffer, 1, 1);
			s_vertexArray->setIndexBuffer(indexBuffer);
			vertexBuffer->drop();
			indexBuffer->drop();
			createdVAO = true;
		}

		RenderCube* cube = new RenderCube(material);
		if (createdVAO) s_vertexArray->drop();
		return cube;
	}

	virtual ~RenderCube() {
		std::lock_guard lock(s_mutex);
		if (s_vertexArray->getRefCount() == 1) {
			s_vertexArray->drop();
			s_vertexArray = nullptr;
		}
		else {
			s_vertexArray->drop();
		}			
	}

	void onRender() override {
		s_vertexArray->bind();
		glDrawElements(GL_TRIANGLES, s_vertexArray->getIndexBuffer()->getIndexCount(), (uint32_t)s_vertexArray->getIndexBuffer()->getIndexType(), nullptr);
	}

	void translate(const glm::vec3& pos) {
		positionMatrix = glm::translate(positionMatrix, pos);
	}

	void rotateAbout(const glm::vec3& axis, float rad) {
		positionMatrix = glm::rotate(positionMatrix, rad, axis);
	}

private:
	static std::mutex s_mutex;
	static pxengine::PxeVertexArray* s_vertexArray;

	RenderCube(pxengine::PxeRenderMaterialInterface& material) : pxengine::PxeRenderObject(material) {
		s_vertexArray->grab();
	}
};

pxengine::PxeVertexArray* RenderCube::s_vertexArray = nullptr;
std::mutex RenderCube::s_mutex;
#endif // !PXENGINESAMPELS_SCENES_SHARED_RENDER_CUBE_H_