#include "RenderShape.h"
namespace pixelexplorer::game::block {
	RenderShape::RenderShape(Shape* shape)
	{
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		_shader = nullptr;
		_material = new rendering::Material();
		_material->setProperty("u_Color", glm::vec4(0, 1, 0, 1));
		positionMatrix = glm::mat4(1.0f);
		_shape = shape;
		_shape->grab();
	}

	RenderShape::~RenderShape()
	{
		_shape->drop();
		_material->drop();
	}

	rendering::Material* RenderShape::getMaterial()
	{
		return _material;
	}

	rendering::Shader* RenderShape::getShader()
	{
		return _shader;
	}

	void RenderShape::createGLObjects(rendering::RenderWindow* window)
	{
		if (getHasGLObjects()) {
			Logger::warn(__FUNCTION__ " Attempted to overwrite GL objects");
			return;
		}

		setHasGLObjects(true);
		_shader = window->loadShader("./assets/shaders/cube.shader");

		// Create and load Vertex Array & Vertex Buffer
		glGenVertexArrays(1, &_vertexArrayGlId);
		glBindVertexArray(_vertexArrayGlId);
		glGenBuffers(1, &_vertexBufferGlId);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferGlId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * _shape->vertexCount, _shape->vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(sizeof(float) * 2));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Create and load index buffer
		glGenBuffers(1, &_indexBufferGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _shape->indexCount * sizeof(uint8_t), _shape->indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void RenderShape::destroyGLObjects(rendering::RenderWindow* window)
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

	void RenderShape::drawMesh()
	{
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glDrawElements(GL_TRIANGLES, _shape->indexCount, GL_UNSIGNED_BYTE, nullptr);
	}
}
