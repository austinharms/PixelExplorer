#include <stdint.h>
#include <mutex>

#include "rendering/GLRenderObject.h"
#include "rendering/Material.h"
#include "DataBuffer.h"
#include "glm/mat4x4.hpp"
#include "rendering/RenderWindow.h"

#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESH_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESH_H_
namespace pixelexplorer::game::chunk {
	class ChunkRenderMesh : public rendering::GLRenderObject
	{
	public:
		ChunkRenderMesh(rendering::Material* material, rendering::RenderWindow* window = nullptr, const glm::vec3& pos = glm::vec3(0));
		virtual ~ChunkRenderMesh();

		void updateMesh(DataBuffer<uint32_t>* indices, DataBuffer<float>* vertices);
		void setPosition(const glm::vec3& pos);

	protected:
		void onInitialize() override;
		void onTerminate() override;
		void onUpdate() override;

	private:
		DataBuffer<uint32_t>* _indexDataBuffer;
		DataBuffer<float>* _vertextDataBuffer;
		std::mutex _dataBufferMutex;
		glm::mat4 _positionMatrix;
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
		uint32_t _indexCount;
	};
}

#endif // !PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESH_H_
