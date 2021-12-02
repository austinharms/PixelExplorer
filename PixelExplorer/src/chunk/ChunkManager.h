#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>
#include <string>

#include "Chunk.h"
#include "RefCounted.h"
#include "glm/vec3.hpp"
#include "rendering/Renderer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

class ChunkManager : public virtual RefCounted {
public:
	ChunkManager(Renderer* renderer);
	~ChunkManager();
	void UnloadChunks();
	void LoadChunk(glm::vec<3, int32_t> pos);
	Chunk* GetChunk(glm::vec<3, int32_t> pos);

private:
	Renderer* _renderer;
	std::unordered_map<glm::vec<3, int32_t>, Chunk*> _chunks;
};

#endif