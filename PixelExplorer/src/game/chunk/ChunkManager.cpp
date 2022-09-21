#include "ChunkManager.h"

#include <stdint.h>

#include "ChunkRenderMesh.h"
#include "common/DataBuffer.h"
#include "common/Logger.h"
#include "../block/FaceDirection.h"
#include "../block/BlockShape.h"
#include "../block/Block.h"
#include "../block/BlockDefinition.h"
#include "../block/BlockFaceDefinition.h"
namespace pixelexplorer::game::chunk {
	ChunkManager::ChunkManager(ChunkRenderMeshFactory& chunkRenderMeshFactory, ChunkGenerator& chunkGenerator, block::BlockManifest& blockManifest) : _renderMeshFactory(chunkRenderMeshFactory), _chunkGenerator(chunkGenerator), _blockManifest(blockManifest) {
		_renderMeshFactory.grab();
		_chunkGenerator.grab();
		_blockManifest.grab();
	}

	ChunkManager::~ChunkManager() {
		_chunkMutex.lock();
		unloadAllChunks();
		_chunkMutex.unlock();
		_renderMeshFactory.drop();
		_chunkGenerator.drop();
		_blockManifest.drop();
	}

	Chunk* ChunkManager::getChunk(const glm::i32vec3& pos, bool forceLoad) {
		// change the scope for the lock guard
		{
			std::lock_guard<std::recursive_mutex> lock(_chunkMutex);
			auto chunk = _loadedChunks.find(pos);
			if (chunk != _loadedChunks.end()) return chunk->second;
		}
		if (forceLoad) return loadChunk(pos);
		return nullptr;
	}

	Chunk* ChunkManager::loadChunk(const glm::i32vec3& pos) {
		ChunkRenderMesh* renderMesh = _renderMeshFactory.getRenderMesh();
		if (renderMesh == nullptr) {
			Logger::error(__FUNCTION__" failed to load Chunk, ChunkRenderMesh was null");
			return nullptr;
		}

		Chunk* chunk;
		// change the scope for the lock guard
		{
			std::lock_guard<std::recursive_mutex> lock(_chunkMutex);
			chunk = new(std::nothrow) Chunk(*renderMesh, pos);
			renderMesh->drop();
			// should this be a fatal log?
			if (chunk == nullptr) {
				Logger::error(__FUNCTION__" failed to load Chunk, failed to allocate Chunk");
				return nullptr;
			}

			_loadedChunks.emplace(pos, chunk);
		}

		_chunkGenerator.generateChunk(*chunk);
		remeshChunk(*chunk);
		return chunk;
	}

	void ChunkManager::unloadChunk(const glm::i32vec3& pos) {
		_chunkMutex.lock();
		ChunkIterator chunkIterator = _loadedChunks.find(pos);
		if (chunkIterator == _loadedChunks.end()) {
			_chunkMutex.unlock();
			Logger::warn(__FUNCTION__" attempted to unload unloaded chunk");
			return;
		}

		unloadChunk(chunkIterator);
		_chunkMutex.unlock();
	}

	void ChunkManager::unloadAllChunks() {
		_chunkMutex.lock();
		auto it = _loadedChunks.begin();
		while (it != _loadedChunks.end())
			unloadChunk(it);
		_chunkMutex.unlock();
	}

	void ChunkManager::updateLoadedChunks(double deltaTime)
	{
		_chunkMutex.lock();
		auto it = _loadedChunks.begin();
		while (it != _loadedChunks.end()) {
			Chunk* chunk = (*it).second;
			chunk->reduceExperationTime(deltaTime);
			if (chunk->getExperationTime() <= 0) {
				unloadChunk(it);
			}
			else {
				++it;
			}
		}

		_chunkMutex.unlock();
	}

	void ChunkManager::loadChunksInRadius(const glm::i32vec3& center, uint16_t radius, double durationSec)
	{
		Chunk* chunk;
		int32_t signedRadius = (int32_t)radius;
		for (int32_t x = -signedRadius; x < signedRadius; ++x) {
			for (int32_t y = -signedRadius; y < signedRadius; ++y) {
				for (int32_t z = -signedRadius; z < signedRadius; ++z) {
					if ((chunk = getChunk(glm::i32vec3(x, y, z) + center, true)) != nullptr) {
						chunk->setExperationTime(durationSec);
					}
				}
			}
		}
	}

	void ChunkManager::remeshChunk(Chunk& chunk) {
		using namespace block;
		chunk.grab();
		// remesh chunk
		uint32_t faceCount = 0;
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
		for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
			const block::Block& block = chunk.getBlock(i);
			if (block.Id != 0) {
				block::BlockDefinition* blockDef;
				if ((blockDef = _blockManifest.getBlock(block.Id)) != nullptr) {
					for (uint8_t faceIndex = 0; faceIndex < 6; ++faceIndex) {
						if (blockDef->getBlockFace((block::FaceDirection)faceIndex)) ++faceCount;
					}
				}
			}

			if (++x == Chunk::CHUNK_SIZE) {
				x = 0;
				if (++z == Chunk::CHUNK_SIZE) {
					z = 0;
					++y;
				}
			}
		}

		uint32_t loadedFaceCount = 0;
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;
		// Add FACE_VERTEX_COUNT here to server as our color per vertex (sizeof(color) == sizeof(float))
		static_assert(sizeof(Color) == sizeof(float));
		DataBuffer<float>* vertextBuffer = new(std::nothrow) DataBuffer<float>((uint64_t)faceCount * (BlockShape::FACE_FLOAT_COUNT + BlockShape::FACE_VERTEX_COUNT));
		DataBuffer<uint32_t>* indexBuffer = new(std::nothrow) DataBuffer<uint32_t>(((uint64_t)faceCount) * BlockShape::FACE_INDEX_COUNT);
		if (vertextBuffer == nullptr || vertextBuffer->getBufferPtr() == nullptr) {
			Logger::error(__FUNCTION__" failed to allocate chunk vertex buffer");
			chunk.drop();
			return;
		}

		if (indexBuffer == nullptr || indexBuffer->getBufferPtr() == nullptr) {
			Logger::error(__FUNCTION__" failed to allocate chunk index buffer");
			chunk.drop();
			return;
		}

		bool buildError = false;

		x = 0;
		y = 0;
		z = 0;
		for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
			const block::Block& block = chunk.getBlock(i);
			if (block.Id != 0) {
				BlockDefinition* blockDef;
				if ((blockDef = _blockManifest.getBlock(block.Id)) != nullptr) {
					const BlockFaceDefinition* blockFace;
					for (uint8_t faceIndex = 0; faceIndex < 6; ++faceIndex) {
						if ((blockFace = blockDef->getBlockFace((FaceDirection)faceIndex))) {
							if (++loadedFaceCount > faceCount) {
								buildError = true;
								Logger::error(__FUNCTION__" failed to build chunk mesh, chunk face count overrun");
								break;
							}

							uint32_t* rawIndexBuffer = indexBuffer->getBufferPtr();
							uint32_t indexOffset = vertexCount / (BlockShape::VERTEX_FLOAT_COUNT + 1);
							for (uint32_t j = 0; j < BlockShape::FACE_INDEX_COUNT; ++j)
								rawIndexBuffer[indexCount++] = BlockShape::BLOCK_FACE_INDICES[faceIndex][j] + indexOffset;

							float* rawVertexBuffer = vertextBuffer->getBufferPtr();
							for (uint32_t j = 0; j < BlockShape::FACE_VERTEX_COUNT; ++j) {
								// we assume that there are 3 floats per vertex (x,y,z)
								static_assert(BlockShape::VERTEX_FLOAT_COUNT == 3);
								rawVertexBuffer[vertexCount++] = BlockShape::BLOCK_FACE_VERTICES[faceIndex][(j * BlockShape::VERTEX_FLOAT_COUNT) + 0] + x;
								rawVertexBuffer[vertexCount++] = BlockShape::BLOCK_FACE_VERTICES[faceIndex][(j * BlockShape::VERTEX_FLOAT_COUNT) + 1] + y;
								rawVertexBuffer[vertexCount++] = BlockShape::BLOCK_FACE_VERTICES[faceIndex][(j * BlockShape::VERTEX_FLOAT_COUNT) + 2] + z;
								((uint32_t*)rawVertexBuffer)[vertexCount++] = blockFace->Color.getColorABGR();
							}
						}
					}
				}
			}

			if (++x == Chunk::CHUNK_SIZE) {
				x = 0;
				if (++z == Chunk::CHUNK_SIZE) {
					z = 0;
					++y;
				}
			}
		}

		if (!buildError) chunk.getRenderMesh().updateMesh(*indexBuffer, *vertextBuffer);
		vertextBuffer->drop();
		indexBuffer->drop();
		chunk.drop();
	}

	void ChunkManager::unloadChunk(ChunkIterator& chunk)
	{
		std::lock_guard<std::recursive_mutex> lock(_chunkMutex);
		if (chunk == _loadedChunks.end()) {
			Logger::warn(__FUNCTION__" attempted to unload invalid ChunkIterator");
			return;
		}

		Chunk* chunkPtr = (*chunk).second;
		_renderMeshFactory.addRenderMesh(chunkPtr->getRenderMesh());
		if (!chunkPtr->drop())
			Logger::warn(__FUNCTION__" chunk manager Chunk not dropped, make sure all other references to the Chunk are dropped");
		chunk = _loadedChunks.erase(chunk);
	}
}