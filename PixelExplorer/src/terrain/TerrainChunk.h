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
			static constexpr float HALF_CHUNK_CELL_SIZE = CHUNK_CELL_SIZE / 2.0f;
			static constexpr float CHUNK_WIDTH = CHUNK_GRID_SIZE * CHUNK_CELL_SIZE;

			static inline constexpr glm::i64vec3 WorldToChunkSpace(const glm::vec3& worldSpace) {
				return glm::i64vec3(
					(int64_t)roundf(worldSpace.x / CHUNK_CELL_SIZE),
					(int64_t)roundf(worldSpace.y / CHUNK_CELL_SIZE),
					(int64_t)roundf(worldSpace.z / CHUNK_CELL_SIZE)
				);
			}

			static inline constexpr int64_t ChunkSpaceToChunkPosition(const int64_t& chunkSpace) {
				if (chunkSpace >= 0) {
					return chunkSpace / CHUNK_GRID_SIZE;
				}
				else {
					return ((chunkSpace + 1) / CHUNK_GRID_SIZE) - 1;
				}
			}

			static inline constexpr glm::i64vec3 ChunkSpaceToChunkPosition(const glm::i64vec3& chunkSpace) {
				return glm::i64vec3(ChunkSpaceToChunkPosition(chunkSpace.x), ChunkSpaceToChunkPosition(chunkSpace.y), ChunkSpaceToChunkPosition(chunkSpace.z));
			}

			static inline constexpr int64_t ChunkSpaceToRelativeChunkSpace(const int64_t& chunkSpace) {
				int64_t res = chunkSpace % CHUNK_GRID_SIZE;

				if (chunkSpace >= 0 || res == 0) {
					return res;
				}
				else {
					return CHUNK_GRID_SIZE + res;
				}
			}

			static inline constexpr glm::i64vec3 ChunkSpaceToRelativeChunkSpace(const glm::i64vec3& chunkSpace) {
				return glm::i64vec3(ChunkSpaceToRelativeChunkSpace(chunkSpace.x), ChunkSpaceToRelativeChunkSpace(chunkSpace.y), ChunkSpaceToRelativeChunkSpace(chunkSpace.z));
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
