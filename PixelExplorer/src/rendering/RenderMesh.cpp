#include "RenderMesh.h"

#include "GL/glew.h"
#include "Logger.h"

namespace pixelexplore::rendering {
	RenderMesh::RenderMesh()
	{
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		indexCount = 0;
		indexType = GL_UNSIGNED_SHORT;
		_hasGlObjects = false;
		_buffersDirty = false;
		positionMatrix = glm::mat4();
	}

	RenderMesh::~RenderMesh()
	{
#ifdef DEBUG
		assert(!_hasGlObjects);
#endif // DEBUG

		if (_hasGlObjects)
			Logger::warn("Render Mesh GL Buffers not deallocated");
	}

	void RenderMesh::updateGlObjects()
	{
		if (!_buffersDirty) return;
		//glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned short), indices, GL_STATIC_DRAW);
		_buffersDirty = false;
	}

	void RenderMesh::drawMesh()
	{
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
	}

	void RenderMesh::deleteGlObjects()
	{
		glDeleteVertexArrays(1, &_vertexArrayGlId);
		glDeleteBuffers(1, &_vertexBufferGlId);
		glDeleteBuffers(1, &_indexBufferGlId);
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		_hasGlObjects = false;
	}

	void RenderMesh::createGlObjects()
	{
		_hasGlObjects = true;
		glGenVertexArrays(1, &_vertexArrayGlId);
		glBindVertexArray(_vertexArrayGlId);
		glGenBuffers(1, &_vertexBufferGlId);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferGlId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glGenBuffers(1, &_indexBufferGlId);
	}
}