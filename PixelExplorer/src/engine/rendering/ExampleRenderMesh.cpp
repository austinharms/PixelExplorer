#include "ExampleRenderMesh.h"

#include <random>

#include "RenderWindow.h"
#include "GL/glew.h"
#include "common/Logger.h"
#include "glm/mat4x4.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/vec4.hpp"

namespace pixelexplorer::engine::rendering {
	ExampleRenderMesh::ExampleRenderMesh()
	{
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		_material = nullptr;
		_positionMatrix = glm::mat4(1.0f);
	}

	ExampleRenderMesh::~ExampleRenderMesh() {}

	void ExampleRenderMesh::onUpdate()
	{
		_material->setProperty("u_Color", glm::vec4((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1));
		RenderWindow* window = getRenderWindow();
		window->setModelMatrix(_positionMatrix);
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
	}

	void ExampleRenderMesh::setPosition(const glm::vec3& pos)
	{
		_positionMatrix[3] = glm::vec4(pos, 1);
	}

	void ExampleRenderMesh::onTerminate()
	{
		_material->drop();
		_material = nullptr;
		glDeleteVertexArrays(1, &_vertexArrayGlId);
		glDeleteBuffers(1, &_vertexBufferGlId);
		glDeleteBuffers(1, &_indexBufferGlId);
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
	}

	void ExampleRenderMesh::onInitialize()
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

		uint16_t indices[36] = {
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

		_material = new Material();
		addDependency(*_material);
		Shader* shader = getRenderWindow()->loadShader("./assets/shaders/cube.shader");
		if (shader != nullptr) {
			addDependency(*shader);
			shader->drop();
			shader = nullptr;
		}
	}
}