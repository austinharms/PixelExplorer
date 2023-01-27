#include "TerrainChunk.h"

#include <string>

#include "SDL_timer.h"

namespace pixelexplorer {
	namespace terrain {
		TerrainChunk::TerrainChunk(const glm::i64vec3& pos, TerrainDropCallback* callback)
		{
			_callback = callback;
			_position = pos;
			_lastModified = 0;
			memset(_points, 0, sizeof(_points));

			updateLastModified();
		}

		const TerrainChunk::ChunkPoint* TerrainChunk::getPoints() const
		{
			return _points;
		}

		TerrainChunk::ChunkPoint* TerrainChunk::getPoints()
		{
			return _points;
		}

		uint64_t TerrainChunk::getLastModified() const
		{
			return _lastModified;
		}

		void TerrainChunk::updateLastModified()
		{
			_lastModified = SDL_GetTicks64();
		}

		const glm::i64vec3& TerrainChunk::getPosition() const
		{
			return _position;
		}

		void TerrainChunk::onDelete()
		{
			if (_callback)
				(*_callback)(*this);
		}

		void TerrainChunk::setPosition(const glm::i64vec3& pos)
		{
			_position = pos;
		}

		void TerrainChunk::setDropCallback(TerrainDropCallback* callback)
		{
			_callback = callback;
		}
	}
}
