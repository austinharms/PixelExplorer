#ifndef PIXELEXPLORER_TERRAIN_TERRAIN_CHUNK_H_
#define PIXELEXPLORER_TERRAIN_TERRAIN_CHUNK_H_
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
			static constexpr float CHUNK_WIDTH = CHUNK_GRID_SIZE * CHUNK_CELL_SIZE;
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
