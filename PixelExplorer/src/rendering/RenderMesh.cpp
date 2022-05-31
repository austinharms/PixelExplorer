#include "RenderMesh.h"

#include "GL/glew.h"
#include "Logger.h"

namespace pixelexplore::rendering {
	RenderMesh::RenderMesh()
	{
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		_shader = nullptr;
	}

	RenderMesh::~RenderMesh() {}

	Shader* RenderMesh::getShader()
	{
		return _shader;
	}

	void RenderMesh::drawMesh()
	{
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}

	void RenderMesh::deleteGlObjects(RenderWindow* window)
	{
		if (!getHasGlObjects()) { 
			Logger::warn(__FUNCTION__ " Attempted to delete empty GL objects");
			return; 
		}

		window->dropShader(_shader);
		glDeleteVertexArrays(1, &_vertexArrayGlId);
		glDeleteBuffers(1, &_vertexBufferGlId);
		glDeleteBuffers(1, &_indexBufferGlId);
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		setHasGlObjects(false);
	}

	void RenderMesh::createGlObjects(RenderWindow* window)
	{
		if (getHasGlObjects()) { 
			Logger::warn( __FUNCTION__ " Attempted to overwrite GL objects");
			return; 
		}

		float vertices[12]{
			-0.5,  0.5, 1, // 0
			-0.5, -0.5, 1, // 1
			 0.5, -0.5, 1, // 2
			 0.5,  0.5, 1  // 3
		};

		uint16_t indices[6]{
			0,1,2,
			0,2,3
		};

		setHasGlObjects(true);
		_shader = window->loadShader("./assets/shaders/base.shader");

		// Create and load Vertex Array & Vertex Buffer
		glGenVertexArrays(1, &_vertexArrayGlId);
		glBindVertexArray(_vertexArrayGlId);
		glGenBuffers(1, &_vertexBufferGlId);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferGlId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
		glEnableVertexAttribArray(0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Create and load index buffer
		glGenBuffers(1, &_indexBufferGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}