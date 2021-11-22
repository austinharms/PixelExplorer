#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <unordered_map>

#include "RefCounted.h"
#include "glm/vec3.hpp"
#include "Chunk.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

class ChunkManager : RefCounted
{
public:
	ChunkManager();
	~ChunkManager();
	void UnloadChunks();
	void LoadChunk(glm::vec<3, int32_t> pos);

private:
	struct KeyTraits {
		size_t operator()(const glm::vec<3, int32_t>& k) const {
			return std::hash<int32_t>()(k.x) ^ std::hash<int32_t>()(k.y);
		}

		bool operator()(const glm::vec<3, int32_t>& a, const glm::vec<3, int32_t>& b) const {
			return a == b;
		}
	};

	typedef std::unordered_map<glm::vec<3, int32_t>, Chunk, KeyTraits, KeyTraits> ChunkMap;
	ChunkMap _chunks;

};

#endif