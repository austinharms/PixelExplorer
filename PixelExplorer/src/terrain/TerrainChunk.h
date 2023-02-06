#ifndef PIXELEXPLORER_TERRAIN_TERRAIN_CHUNK_H_
#define PIXELEXPLORER_TERRAIN_TERRAIN_CHUNK_H_
#include <math.h>

#include "PxeRefCount.h"
#include "glm/vec3.hpp"

namespace pixelexplorer {
	namespace terrain {
		class TerrainManager;

		class TerrainChunk : public pxengine::PxeRefCount
		{
		public:
			static constexpr uint32_t CHUNK_GRID_SIZE = 15;
			static constexpr uint32_t CHUNK_LAYER_POINT_COUNT = CHUNK_GRID_SIZE * CHUNK_GRID_SIZE;
			static constexpr uint32_t CHUNK_POINT_COUNT = CHUNK_LAYER_POINT_COUNT * CHUNK_GRID_SIZE;
			static constexpr float CHUNK_CELL_SIZE = 1;
			static constexpr float CHUNK_POSITION_SCALE_FACTOR = 1.0f / CHUNK_CELL_SIZE;
			static constexpr float CHUNK_WIDTH = CHUNK_GRID_SIZE * CHUNK_CELL_SIZE;

			static inline constexpr glm::i64vec3 WorldToChunkSpace(const glm::vec3& worldSpace) { 
				return glm::i64vec3(
					(int64_t)roundf(worldSpace.x * CHUNK_POSITION_SCALE_FACTOR), 
					(int64_t)roundf(worldSpace.y * CHUNK_POSITION_SCALE_FACTOR), 
					(int64_t)roundf(worldSpace.z * CHUNK_POSITION_SCALE_FACTOR)
				);
			}

			static inline constexpr glm::i64vec3 ChunkSpaceToChunkPosition(const glm::i64vec3& chunkSpace) {
				glm::i64vec3 chunkPos(chunkSpace.x / CHUNK_GRID_SIZE, chunkSpace.y / CHUNK_GRID_SIZE, chunkSpace.z / CHUNK_GRID_SIZE);
				if (chunkSpace.x < 0) chunkPos.x -= 1;
				if (chunkSpace.y < 0) chunkPos.y -= 1;
				if (chunkSpace.z < 0) chunkPos.z -= 1;
				return chunkPos;
			}

			static inline constexpr glm::i64vec3 ChunkSpaceToRelativeChunkSpace(const glm::i64vec3& chunkSpace) {
				glm::i64vec3 chunkPos(chunkSpace.x % CHUNK_GRID_SIZE, chunkSpace.y % CHUNK_GRID_SIZE, chunkSpace.z % CHUNK_GRID_SIZE);
				if (chunkSpace.x < 0) chunkPos.x += CHUNK_GRID_SIZE - 1;
				if (chunkSpace.y < 0) chunkPos.y += CHUNK_GRID_SIZE - 1;
				if (chunkSpace.z < 0) chunkPos.z += CHUNK_GRID_SIZE - 1;
				return chunkPos;
			}

			static inline constexpr uint32_t RelativeChunkSpaceToPointIndex(const glm::i64vec3& relChunkSpace) {
				return relChunkSpace.x + relChunkSpace.z * CHUNK_GRID_SIZE + relChunkSpace.y * CHUNK_LAYER_POINT_COUNT;
			}

			typedef uint16_t ChunkPoint;

			virtual ~TerrainChunk() = default;
			const ChunkPoint* getPoints() const;
			ChunkPoint* getPoints();
			uint64_t getLastModified() const;
			void updateLastModified();
			const glm::i64vec3& getPosition() const;

		protected:
			void onDelete() override;

		private:
			friend class TerrainManager;
			class TerrainDropCallback { public: virtual void operator()(TerrainChunk& chunk) = 0; };
			TerrainChunk(const glm::i64vec3& pos = glm::i64vec3(0), TerrainDropCallback* callback = nullptr);
			void setPosition(const glm::i64vec3& pos);
			void setDropCallback(TerrainDropCallback* callback);

			TerrainDropCallback* _callback;
			glm::i64vec3 _position;
			uint64_t _lastModified;
			ChunkPoint _points[CHUNK_POINT_COUNT];
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_TERRAIN_CHUNK_H_
