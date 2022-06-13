#include <stdint.h>

#include "rendering/RenderObject.h"
#include "rendering/Shader.h"
#include "rendering/Material.h"

#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESH_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESH_H_
namespace pixelexplorer::game::chunk {
	class ChunkRenderMesh : public rendering::RenderObject
	{
	public:
		ChunkRenderMesh(rendering::Shader* shader = nullptr, rendering::Material* material = nullptr);
		virtual ~ChunkRenderMesh();
		rendering::Material* getMaterial();
		rendering::Shader* getShader();
		void createGLObjects();
		void destroyGLObjects();
		void drawMesh();

	private:
		rendering::Shader* _shader;
		rendering::Material* _material;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
	};
}

#endif // !PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESH_H_
