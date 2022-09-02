#include "ChunkRenderMesh.h"
#include "Logger.h"
#include "GL/glew.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "rendering/RenderWindow.h"

namespace pixelexplorer::game::chunk {
	ChunkRenderMesh::ChunkRenderMesh(rendering::Material* material, rendering::RenderWindow* window, const glm::vec3& pos)
	{
		if (window != nullptr)
			window->registerGLObject(this);
		addDependency(material);
		_vertextDataBuffer = nullptr;
		_indexDataBuffer = nullptr;
		_indexBufferGlId = 0;
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_positionMatrix = glm::mat4(1.0f);
		_indexCount = 0;
		setPosition(pos);
	}

	ChunkRenderMesh::~ChunkRenderMesh()
	{
		_dataBufferMutex.lock();
		if (_vertextDataBuffer != nullptr) {
			_vertextDataBuffer->drop();
			_vertextDataBuffer = nullptr;
		}

		if (_indexDataBuffer != nullptr) {
			_indexDataBuffer->drop();
			_indexDataBuffer = nullptr;
		}

		_dataBufferMutex.unlock();
	}

	void ChunkRenderMesh::updateMesh(DataBuffer<uint32_t>* indices, DataBuffer<float>* vertices)
	{
		if (indices == nullptr || vertices == nullptr) {
			Logger::warn(__FUNCTION__" indices and or vertices was null, ChunkRenderMesh not updated");
			return;
		}

		indices->grab();
		vertices->grab();

		_dataBufferMutex.lock();
		if (_vertextDataBuffer != nullptr)
			_vertextDataBuffer->drop();
		_vertextDataBuffer = vertices;

		if (_indexDataBuffer != nullptr)
			_indexDataBuffer->drop();
		_indexDataBuffer = indices;
		_dataBufferMutex.unlock();
		setDirty();
	}

	void ChunkRenderMesh::setPosition(const glm::vec3& pos)
	{
		_positionMatrix[3] = glm::vec4(pos, 1);
	}

	void ChunkRenderMesh::onInitialize()
	{
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

		uint32_t indices[36] = {
			0, 2, 1, 0, 3, 2,  // Front
			4, 5, 6, 4, 6, 7,  // Back
			0, 4, 7, 0, 7, 3,  // Left
			1, 6, 5, 1, 2, 6,  // Right
			3, 7, 6, 3, 6, 2,  // Top
			0, 5, 4, 0, 1, 5,  // Bottom
		};

		// Create and load Vertex Array & Vertex Buffer
		glGenVertexArrays(1, &_vertexArrayGlId);
		glBindVertexArray(_vertexArrayGlId);
		glGenBuffers(1, &_vertexBufferGlId);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferGlId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Create and load index buffer
		glGenBuffers(1, &_indexBufferGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		_indexCount = 36;
	}

	void ChunkRenderMesh::onTerminate()
	{
		glDeleteVertexArrays(1, &_vertexArrayGlId);
		glDeleteBuffers(1, &_vertexBufferGlId);
		glDeleteBuffers(1, &_indexBufferGlId);
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
	}

	void ChunkRenderMesh::onUpdate()
	{
		getRenderWindow()->setModelMatrix(_positionMatrix);
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);

		// check if mesh needs update
		if (getDirty()) {
			_dataBufferMutex.lock();
			glBufferData(GL_ARRAY_BUFFER, _vertextDataBuffer->getSize(), _vertextDataBuffer->getBufferPtr(), GL_STATIC_DRAW);
			_vertextDataBuffer->drop();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexDataBuffer->getSize(), _indexDataBuffer->getBufferPtr(), GL_STATIC_DRAW);
			_indexCount = _indexDataBuffer->getLength();
			_indexDataBuffer->drop();
			_dataBufferMutex.unlock();
			clearDirty();
		}

		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
	}
}