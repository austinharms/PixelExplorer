#include <stdint.h>

#include "Shape.h"
#include "rendering/GLRenderObject.h"
#include "rendering/Shader.h"
#include "rendering/RenderWindow.h"
#include "rendering/Material.h"

#ifndef PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
#define PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
namespace pixelexplorer::game::block {
	class RenderShape : public rendering::GLRenderObject
	{
	public:
		RenderShape(Shape* shape);
		virtual ~RenderShape();

	protected:
		void onInitialize() override;
		void onTerminate() override;
		void onUpdate() override;

	private:
		Shape* _shape;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}
#endif // !PIEXELEXPLORER_GAME_BLOCK_RENDERSHAPE_H_
