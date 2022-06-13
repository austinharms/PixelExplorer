#include <stdint.h>

#include "Shape.h"
#include "rendering/RenderObject.h"
#include "rendering/Shader.h"
#include "rendering/RenderWindow.h"
#include "rendering/Material.h"

#ifndef PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
#define PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
namespace pixelexplorer::game::block {
	class RenderShape : public rendering::RenderObject
	{
		friend class RenderWindow;
	public:
		RenderShape(Shape* shape);
		virtual ~RenderShape();
		rendering::Material* getMaterial();
		rendering::Shader* getShader();

	protected:
		void createGLObjects();
		void destroyGLObjects();
		void drawMesh();

	private:
		rendering::Shader* _shader;
		Shape* _shape;
		rendering::Material* _material;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}
#endif // !PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
