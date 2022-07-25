#include <stdint.h>

#include "Shape.h"
#include "rendering/BasicGLRenderObject.h"
#include "rendering/Shader.h"
#include "rendering/RenderWindow.h"
#include "rendering/Material.h"

#ifndef PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
#define PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
namespace pixelexplorer::game::block {
	class RenderShape : public rendering::BasicGLRenderObject
	{
	public:
		RenderShape(Shape* shape);
		virtual ~RenderShape();

	protected:
		void onInitialize() override;
		void onTerminate() override;
		void onRender() override;

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
