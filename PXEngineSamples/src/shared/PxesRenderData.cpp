#include "PxesRenderData.h"

#include <cassert>

#include "PxeVertexBuffer.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBufferFormat.h"
#include "PxeVertexBufferAttrib.h"
#include "PxeFixedBuffer.h"
#include "CubeData.h"
#include "GL/glew.h"

PxesRenderData::PxesRenderData() {
	pxengine::PxeVertexBufferFormat format;
	assert(cubeRenderVertexStride == 5 && "PxesRenderData's PxeVertexBufferAttribs need updating");
	format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
	format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 2, false));
	pxengine::PxeVertexBuffer* vertexBuffer = new pxengine::PxeVertexBuffer(format);
	pxengine::PxeBuffer* vertexData = pxengine::PxeFixedBuffer::create(sizeof(cubeRenderVertices));
	assert(vertexData);
	memcpy(vertexData->getBuffer(), cubeRenderVertices, vertexData->getSize());
	vertexBuffer->bufferData(*vertexData);
	vertexData->drop();

	// Create and load index buffer
	assert(cubeRenderIndexCount * 2 == sizeof(cubeRenderIndices) && "PxesRenderData's PxeIndexType need updating");
	pxengine::PxeIndexBuffer* indexBuffer = new pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_16BIT);
	pxengine::PxeBuffer* indexData = pxengine::PxeFixedBuffer::create(sizeof(cubeRenderIndices));
	assert(indexData);
	memcpy(indexData->getBuffer(), cubeRenderIndices, indexData->getSize());
	indexBuffer->bufferData(*indexData);
	indexData->drop();

	// Attach buffers to VAO
	_vertexArray = new pxengine::PxeVertexArray();
	_vertexArray->addVertexBuffer(*vertexBuffer, 0, 0);
	_vertexArray->addVertexBuffer(*vertexBuffer, 1, 1);
	_vertexArray->setIndexBuffer(indexBuffer);
	vertexBuffer->drop();
	indexBuffer->drop();
}

PxesRenderData::~PxesRenderData() { _vertexArray->drop(); }

void PxesRenderData::draw() {
	_vertexArray->bind();
	glDrawElements(GL_TRIANGLES, _vertexArray->getIndexBuffer()->getIndexCount(), (uint32_t)_vertexArray->getIndexBuffer()->getIndexType(), nullptr);
	_vertexArray->unbind();
}