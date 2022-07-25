#include "RenderShape.h"

#include "rendering/RenderWindow.h"

namespace pixelexplorer::game::block {
	RenderShape::RenderShape(Shape* shape)
	{
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
		_shader = nullptr;
		_material = new rendering::Material();
		_material->setProperty("u_Color", glm::vec4(0, 1, 0, 1));
		_shape = shape;
		_shape->grab();
	}

	RenderShape::~RenderShape()
	{
		_shape->drop();
		_material->drop();
	}

	void RenderShape::onInitialize()
	{
		_shader = getRenderWindow()->getShader("./assets/shaders/cube.shader");

		// Create and load Vertex Array & Vertex Buffer
		glGenVertexArrays(1, &_vertexArrayGlId);
		glBindVertexArray(_vertexArrayGlId);
		glGenBuffers(1, &_vertexBufferGlId);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferGlId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * _shape->vertexCount[0], _shape->vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(sizeof(float) * 2));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Create and load index buffer
		glGenBuffers(1, &_indexBufferGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _shape->indexCount[0] * sizeof(uint8_t), _shape->indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void RenderShape::onTerminate()
	{
		if (_shader != nullptr)
			_shader->drop();
		_shader = nullptr;
		glDeleteVertexArrays(1, &_vertexArrayGlId);
		glDeleteBuffers(1, &_vertexBufferGlId);
		glDeleteBuffers(1, &_indexBufferGlId);
		_vertexArrayGlId = 0;
		_vertexBufferGlId = 0;
		_indexBufferGlId = 0;
	}

	void RenderShape::onRender()
	{
		if (_shader == nullptr) return;
		rendering::RenderWindow* window = getRenderWindow();
		window->setShader(_shader);
		_material->applyMaterial(_shader);
		window->setModelMatrix(glm::mat4(1.0));
		glBindVertexArray(_vertexArrayGlId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferGlId);
		glDrawElements(GL_TRIANGLES, _shape->indexCount[0], GL_UNSIGNED_BYTE, nullptr);
	}
}
