#include "RenderMesh.h"

#include <random>

#include "GL/glew.h"
#include "Logger.h"
#include "glm/mat4x4.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/vec4.hpp"

namespace pixelexplorer::rendering {
	RenderMesh::RenderMesh()
	{
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		_shader = nullptr;
		_material = new Material();
		_material->setProperty("u_Color", glm::vec4(1, 1, 1, 1));
		positionMatrix = glm::mat4(1.0f);
	}

	RenderMesh::~RenderMesh() {
		_material->drop();
	}

	Shader* RenderMesh::getShader()
	{
		return _shader;
	}

	void RenderMesh::drawMesh()
	{
		_material->setProperty("u_Color", glm::vec4((float)rand()/RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1));
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
	}

	void RenderMesh::setPosition(const glm::vec3& pos)
	{
		positionMatrix[3] = glm::vec4(pos, 1);
	}

	void RenderMesh::destroyGLObjects(RenderWindow* window)
	{
		if (!getHasGLObjects()) { 
			Logger::warn(__FUNCTION__ " Attempted to delete empty GL objects");
			return; 
		}

		window->dropShader(_shader);
		_shader = nullptr;
		glDeleteVertexArrays(1, &_vertexArrayGlId);
		glDeleteBuffers(1, &_vertexBufferGlId);
		glDeleteBuffers(1, &_indexBufferGlId);
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		setHasGLObjects(false);
	}

	void RenderMesh::createGLObjects(RenderWindow* window)
	{
		if (getHasGLObjects()) { 
			Logger::warn( __FUNCTION__ " Attempted to overwrite GL objects");
			return; 
		}

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

		setHasGLObjects(true);
		_shader = window->loadShader("./assets/shaders/cube.shader");

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
	}

	Material* RenderMesh::getMaterial()
	{
		return _material;
	}
}