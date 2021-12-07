#include "ChunkManager.h"

#include <iostream>

ChunkManager::ChunkManager(Renderer* renderer) {
	_renderer = renderer;
	renderer->grab();
}

ChunkManager::~ChunkManager() {
	UnloadChunks();
	_renderer->drop();
}

void ChunkManager::UnloadChunks() {
	for (std::pair<const glm::vec<3, int32_t>, Chunk*>& pair : _chunks) {
		pair.second->setStatus(Chunk::Status::UNLOADED);
		pair.second->setRendererDropFlag(true);
		pair.second->drop();
	}

	_chunks.clear();
}

void ChunkManager::LoadChunk(const glm::vec<3, int32_t> pos) {
	Chunk* newChunk = new Chunk();
	_chunks.insert({ pos, newChunk });
	_renderer->addRenderable(newChunk);
	newChunk->setStatus(Chunk::Status::LOADING);
	newChunk->setPosition(pos);
	newChunk->setStatus(Chunk::Status::LOADED);
	newChunk->updateAdjacentChunks(this);
	newChunk->updateMesh();
	UpdateLoadedChunks();
}

Chunk* ChunkManager::GetChunk(glm::vec<3, int32_t> pos) {
	try {
	  return _chunks.at(pos);
	} catch (...) {
	  return nullptr;
	}
	//for (auto& ch : _chunks)
	//	if (ch.first == pos)
	//		return ch.second;
	//return nullptr;
}

void ChunkManager::UpdateLoadedChunks()
{
	for (auto& ch : _chunks)
		ch.second->updateMesh();
}
