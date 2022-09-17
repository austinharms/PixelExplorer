#include <stdint.h>
#include <unordered_map>
#include <queue>
#include <mutex>

#include "common/OSHelpers.h"
#include "common/Logger.h"
#include "common/RefCount.h"
#include "common/DataBuffer.h"
#include "engine/rendering/Material.h"
#include "engine/rendering/Shader.h"
#include "engine/rendering/RenderWindow.h"
#include "ChunkRenderMesh.h"
#include "Chunk.h"
#include "../block/Block.h"
#include "../block/BlockDefinition.h"
#include "../block/BlockFaceDefinition.h"
#include "../block/BlockManifest.h"
#include "../block/FaceDirection.h"
#include "../block/BlockShape.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"


#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
namespace pixelexplorer::game::chunk {
	class ChunkManager : public RefCount
	{
	public:
		inline ChunkManager(engine::rendering::RenderWindow& window, block::BlockManifest& blockManifest) : _renderWindow(window), _blockManifest(blockManifest), _chunkMaterial(*(new engine::rendering::Material())) {
			_renderWindow.grab();
			_blockManifest.grab();
			_renderWindow.registerGLObject(_chunkMaterial);
			// TODO add check that shader is not null
			engine::rendering::Shader* chunkShader = _renderWindow.loadShader(OSHelper::getAssetPath("shaders") / "chunk.shader");
			_chunkMaterial.addDependency(*chunkShader);
			chunkShader->drop();
			chunkShader = nullptr;
		}

		inline virtual ~ChunkManager() {
			_chunkMutex.lock();
			unloadAllChunks();
			dropUnusedRenderMeshes();
			_chunkMutex.unlock();
			_chunkMaterial.drop();
			_renderWindow.drop();
			_blockManifest.drop();
		}

		inline Chunk* getChunk(const glm::i32vec3& pos, bool forceLoad = false) {
			std::lock_guard<std::recursive_mutex> lock(_chunkMutex);
			auto chunk = _loadedChunks.find(pos);
			if (chunk != _loadedChunks.end()) return chunk->second;
			if (forceLoad) return loadChunk(pos);
			return nullptr;
		}

		inline Chunk* loadChunk(const glm::i32vec3& pos) {
			ChunkRenderMesh* renderMesh;
			Chunk* chunk;
			// change the scope for the lock guard
			{
				std::lock_guard<std::recursive_mutex> lock(_chunkMutex);
				if (_unusedRenderMeshes.empty()) {
					renderMesh = new ChunkRenderMesh(_chunkMaterial, &_renderWindow);
					if (renderMesh == nullptr) {
						Logger::error(__FUNCTION__" failed to allocate ChunkRenderMesh object");
						return nullptr;
					}
				}
				else {
					renderMesh = _unusedRenderMeshes.front();
					_unusedRenderMeshes.pop();
				}

				chunk = new Chunk(*renderMesh, pos);
				renderMesh->drop();
				// should this be a fatal log?
				if (chunk == nullptr) {
					Logger::error(__FUNCTION__" failed to allocate chunk object");
					return nullptr;
				}

				_loadedChunks.emplace(pos, chunk);
			}

			remeshChunk(*chunk);
			_renderWindow.addGLRenderObject(*renderMesh);
			return chunk;
		}

		inline void unloadChunk(const glm::i32vec3& pos) {
			_chunkMutex.lock();
			auto chunkIterator = _loadedChunks.find(pos);
			if (chunkIterator == _loadedChunks.end()) {
				_chunkMutex.unlock();
				Logger::warn(__FUNCTION__" attempted to unload unloaded chunk");
				return;
			}

			Chunk* chunk = chunkIterator->second;
			_loadedChunks.erase(chunkIterator);
			ChunkRenderMesh& renderMesh = chunk->getRenderMesh();
			renderMesh.grab();
			_renderWindow.removeGLRenderObject(renderMesh);
			_unusedRenderMeshes.emplace(&renderMesh);
			_chunkMutex.unlock();
			if (!chunk->drop()) Logger::warn(__FUNCTION__" loaded chunk not dropped");
			chunk = nullptr;
		}

		inline void unloadAllChunks() {
			_chunkMutex.lock();
			auto it = _loadedChunks.begin();
			while (it != _loadedChunks.end()) {
				const glm::i32vec3 chunkPos = it->first;
				it = ++it;
				unloadChunk(chunkPos);
			}

			_chunkMutex.unlock();
		}

		inline void dropUnusedRenderMeshes() {
			_chunkMutex.lock();
			while (!_unusedRenderMeshes.empty()) {
				_unusedRenderMeshes.front()->drop();
				_unusedRenderMeshes.pop();
			}

			_chunkMutex.unlock();
		}

		inline void remeshChunk(Chunk& chunk) {
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

	private:
		engine::rendering::RenderWindow& _renderWindow;
		block::BlockManifest& _blockManifest;
		engine::rendering::Material& _chunkMaterial;
		std::unordered_map<glm::i32vec3, Chunk*> _loadedChunks;
		std::queue<ChunkRenderMesh*> _unusedRenderMeshes;
		std::recursive_mutex _chunkMutex;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
