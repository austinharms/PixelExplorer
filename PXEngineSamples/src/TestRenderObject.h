#include <new>

#include "PxeIndexBuffer.h"
#include "PxeVertexBuffer.h"
#include "PxeVertexArray.h"
#include "PxeVertexBufferAttrib.h"
#include "PxeVertexBufferFormat.h"
#include "PxeRenderObject.h"

#ifndef PXENGINESAMPLES_TEST_RENDER_OBJECT_H_
#define PXENGINESAMPLES_TEST_RENDER_OBJECT_H_

class TestRenderObject : public pxengine::PxeRenderObject
{
public:
	TestRenderObject(pxengine::PxeRenderMaterial& material) : pxengine::PxeRenderObject(material) {
		_indexBuffer = new(std::nothrow) pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_8BIT);
		_indexBuffer->initializeAsset();
		pxengine::PxeVertexBufferFormat format(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
		_vertexBuffer = new(std::nothrow) pxengine::PxeVertexBuffer(format);
		_vertexBuffer->initializeAsset();
		_vertexArray = new(std::nothrow) pxengine::PxeVertexArray();
		_vertexArray->addVertexBuffer(*_vertexBuffer, 0, 0);
		_vertexArray->initializeAsset();

		float vertices[24] = {
		 -0.5f, -0.5f, -0.5f,  // 0
		  0.5f, -0.5f, -0.5f,  // 1
		  0.5f,  0.5f, -0.5f,  // 2
		 -0.5f,  0.5f, -0.5f,  // 3
		 -0.5f, -0.5f,  0.5f,  // 4
		  0.5f, -0.5f,  0.5f,  // 5
		  0.5f,  0.5f,  0.5f,  // 6
		 -0.5f,  0.5f,  0.5f   // 7
		};

		uint8_t indices[36] = {
			0, 2, 1, 0, 3, 2,  // Front
			4, 5, 6, 4, 6, 7,  // Back
			0, 4, 7, 0, 7, 3,  // Left
			1, 6, 5, 1, 2, 6,  // Right
			3, 7, 6, 3, 6, 2,  // Top
			0, 5, 4, 0, 1, 5,  // Bottom
		};

		pxengine::PxeBuffer* indexBuffer = new(std::nothrow) pxengine::PxeBuffer(36);
		memcpy(indexBuffer->getBuffer(), indices, indexBuffer->getSize());
		pxengine::PxeBuffer* vertexBuffer = new(std::nothrow) pxengine::PxeBuffer(24 * sizeof(float));
		memcpy(vertexBuffer->getBuffer(), vertices, vertexBuffer->getSize());
		_vertexBuffer->bufferData(*vertexBuffer);
		_indexBuffer->bufferData(*indexBuffer);
		indexBuffer->drop();
		vertexBuffer->drop();
		indexBuffer = nullptr;
		vertexBuffer = nullptr;
	}

	virtual ~TestRenderObject() {
		_indexBuffer->drop();
		_vertexArray->drop();
		_vertexBuffer->drop();
	}

	void render() override {
		_vertexArray->bind();
		_indexBuffer->bind();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
		_indexBuffer->unbind();
		_vertexArray->unbind();
	}

private:
	pxengine::PxeIndexBuffer* _indexBuffer;
	pxengine::PxeVertexBuffer* _vertexBuffer;
	pxengine::PxeVertexArray* _vertexArray;
};

#endif // !PXENGINESAMPLES_TEST_RENDER_OBJECT_H_
