#include "Chunk.h"

#include <iostream>

#include "chunk/block/Block.h"

void* Chunk::s_emptyBuffer = malloc(1);
TexturedMaterial* Chunk::s_chunkMaterial = nullptr;

Chunk::Chunk() : Renderable(Chunk::GetChunkMaterial()) {
	setPosition(glm::vec3(0));
	_status = Status::CREATED;
	_visible = true;
	_vertexBuffer = nullptr;
	_vertexCount = 0;
	_chunkModified = false;
	for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
		_buffers[i] = nullptr;
		_indexCount[i] = 0;
		_currentIndexCount[i] = 0;
	}

	_buffersDirty = false;
	glGenVertexArrays(1, &_vertexArrayId);
	glBindVertexArray(_vertexArrayId);
	glGenBuffers(1, &_vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
		(const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers((int)FaceDirection::FACECOUNT, _indexBuffers);
	memset(_blocks, 0, Chunk::BLOCK_COUNT * sizeof(ChunkBlock));
	for (int32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) _blocks[i].setId(1);
	_chunkModified = true;
	_blocks[0] = 0;
}

Chunk::~Chunk() {}

bool Chunk::onPreRender(float deltaTime, float* cameraPos,
	float* cameraRotation) {
	if (_status < Status::LOADED) return false;
	if (_buffersDirty) updateBuffers();
	_visibleFaces = FaceDirectionFlag::ALL;
	return _visible;
}

void Chunk::onRender() {
	glBindVertexArray(_vertexArrayId);
	for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
		if (!(_visibleFaces & FaceDirectionFlag::DirectionToFlag(i)) ||
			!_currentIndexCount[i])
			continue;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[i]);
		glDrawElements(GL_TRIANGLES, _currentIndexCount[i], GL_UNSIGNED_INT,
			nullptr);
	}
}

void Chunk::updateMesh() {
	if (!_chunkModified) return;
	std::lock_guard<std::mutex> lock(_meshBuffersLock);
	_chunkModified = false;

	// Clear Old Buffer Data
	if (_vertexBuffer != nullptr) {
		if (_vertexBuffer != Chunk::s_emptyBuffer) free(_vertexBuffer);
		_vertexBuffer = nullptr;
	}

	_vertexCount = 0;

	for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
		if (_buffers[i] != nullptr) {
			if (_buffers[i] != Chunk::s_emptyBuffer) free(_buffers[i]);
			_buffers[i] = nullptr;
		}

		_indexCount[i] = 0;
	}

	float x = 0;
	float y = 0;
	float z = 0;
	Block curBlock(this);
	Block otherBlock(this);
	uint8_t faceNum;
	uint8_t visableFaces[Chunk::BLOCK_COUNT];
	memset(visableFaces, FaceDirectionFlag::NONE, Chunk::BLOCK_COUNT);

	// Find How Big the Mesh Buffers Should Be And What Faces Are Visable
	for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
		curBlock.setChunkBlock(&_blocks[i], i);
		if (!curBlock.isEmpty()) {
			if (z != 0) {
				otherBlock.setChunkBlock(&_blocks[i - Chunk::CHUNK_SIZE],
					i - Chunk::CHUNK_SIZE, this);
			}
			else if (_adjacentChunks[(uint8_t)FaceDirection::FRONT] != nullptr) {
				otherBlock.setChunkBlock(&_adjacentChunks[(uint8_t)FaceDirection::FRONT]->_blocks[i + Chunk::LAYER_SIZE - Chunk::CHUNK_SIZE], i + Chunk::LAYER_SIZE - Chunk::CHUNK_SIZE, _adjacentChunks[(uint8_t)FaceDirection::FRONT]);
			}

			if (z == 0 && _adjacentChunks[(uint8_t)FaceDirection::FRONT] == nullptr || otherBlock.isEmpty() ||
				(!otherBlock.isSolid() &&
					(!otherBlock.isFaceFull(FaceDirection::BACK) ||
						otherBlock.isFaceTransparent(FaceDirection::BACK) &&
						otherBlock.getId() != curBlock.getId()))) {
				faceNum = (uint8_t)FaceDirection::FRONT;
				const BlockFace* face = curBlock.getBlockFace(FaceDirection::FRONT);
				_vertexCount += face->vertexCount;
				_indexCount[faceNum] += face->indexCount;
				visableFaces[i] |= FaceDirectionFlag::FRONT;
			}

			if (z != Chunk::CHUNK_SIZE - 1) {
				otherBlock.setChunkBlock(&_blocks[i + Chunk::CHUNK_SIZE],
					i + Chunk::CHUNK_SIZE, this);
			}
			else if (_adjacentChunks[(uint8_t)FaceDirection::BACK] != nullptr) {
				otherBlock.setChunkBlock(&_adjacentChunks[(uint8_t)FaceDirection::BACK]->_blocks[i - Chunk::LAYER_SIZE + Chunk::CHUNK_SIZE], i - Chunk::LAYER_SIZE + Chunk::CHUNK_SIZE, _adjacentChunks[(uint8_t)FaceDirection::BACK]);
			}

			if (z == Chunk::CHUNK_SIZE - 1 && _adjacentChunks[(uint8_t)FaceDirection::BACK] == nullptr || otherBlock.isEmpty() ||
				(!otherBlock.isSolid() &&
					(!otherBlock.isFaceFull(FaceDirection::FRONT) ||
						otherBlock.isFaceTransparent(FaceDirection::FRONT) &&
						otherBlock.getId() != curBlock.getId()))) {
				faceNum = (uint8_t)FaceDirection::BACK;
				const BlockFace* face = curBlock.getBlockFace(FaceDirection::BACK);
				_vertexCount += face->vertexCount;
				_indexCount[faceNum] += face->indexCount;
				visableFaces[i] |= FaceDirectionFlag::BACK;
			}

			if (x != 0) {
				otherBlock.setChunkBlock(&_blocks[i - 1], i - 1, this);
			}
			else if (_adjacentChunks[(uint8_t)FaceDirection::LEFT] != nullptr) {
				otherBlock.setChunkBlock(&_adjacentChunks[(uint8_t)FaceDirection::LEFT]->_blocks[i + Chunk::CHUNK_SIZE - 1], i + Chunk::CHUNK_SIZE - 1, _adjacentChunks[(uint8_t)FaceDirection::LEFT]);
			}

			if (x == 0 && _adjacentChunks[(uint8_t)FaceDirection::LEFT] == nullptr || otherBlock.isEmpty() ||
				(!otherBlock.isSolid() &&
					(!otherBlock.isFaceFull(FaceDirection::RIGHT) ||
						otherBlock.isFaceTransparent(FaceDirection::RIGHT) &&
						otherBlock.getId() != curBlock.getId()))) {
				faceNum = (uint8_t)FaceDirection::LEFT;
				const BlockFace* face = curBlock.getBlockFace(FaceDirection::LEFT);
				_vertexCount += face->vertexCount;
				_indexCount[faceNum] += face->indexCount;
				visableFaces[i] |= FaceDirectionFlag::LEFT;
			}

			if (x != Chunk::CHUNK_SIZE - 1) {
				otherBlock.setChunkBlock(&_blocks[i + 1], i + 1, this);
			}
			else if (_adjacentChunks[(uint8_t)FaceDirection::RIGHT] != nullptr) {
				otherBlock.setChunkBlock(&_adjacentChunks[(uint8_t)FaceDirection::RIGHT]->_blocks[i - Chunk::CHUNK_SIZE + 1], i - Chunk::CHUNK_SIZE + 1, _adjacentChunks[(uint8_t)FaceDirection::RIGHT]);
			}

			if (x == Chunk::CHUNK_SIZE - 1 && _adjacentChunks[(uint8_t)FaceDirection::RIGHT] == nullptr || otherBlock.isEmpty() ||
				(!otherBlock.isSolid() &&
					(!otherBlock.isFaceFull(FaceDirection::LEFT) ||
						otherBlock.isFaceTransparent(FaceDirection::LEFT) &&
						otherBlock.getId() != curBlock.getId()))) {
				faceNum = (uint8_t)FaceDirection::RIGHT;
				const BlockFace* face = curBlock.getBlockFace(FaceDirection::RIGHT);
				_vertexCount += face->vertexCount;
				_indexCount[faceNum] += face->indexCount;
				visableFaces[i] |= FaceDirectionFlag::RIGHT;
			}

			if (y != Chunk::CHUNK_SIZE - 1) {
				otherBlock.setChunkBlock(&_blocks[i + Chunk::LAYER_SIZE],
					i + Chunk::LAYER_SIZE, this);
			}
			else if (_adjacentChunks[(uint8_t)FaceDirection::TOP] != nullptr) {
				otherBlock.setChunkBlock(&_adjacentChunks[(uint8_t)FaceDirection::TOP]->_blocks[i - Chunk::BLOCK_COUNT + Chunk::LAYER_SIZE], i - Chunk::BLOCK_COUNT + Chunk::LAYER_SIZE, _adjacentChunks[(uint8_t)FaceDirection::TOP]);
			}

			if (y == Chunk::CHUNK_SIZE - 1 && _adjacentChunks[(uint8_t)FaceDirection::TOP] == nullptr || otherBlock.isEmpty() ||
				(!otherBlock.isSolid() &&
					(!otherBlock.isFaceFull(FaceDirection::BOTTOM) ||
						otherBlock.isFaceTransparent(FaceDirection::BOTTOM) &&
						otherBlock.getId() != curBlock.getId()))) {
				faceNum = (uint8_t)FaceDirection::TOP;
				const BlockFace* face = curBlock.getBlockFace(FaceDirection::TOP);
				_vertexCount += face->vertexCount;
				_indexCount[faceNum] += face->indexCount;
				visableFaces[i] |= FaceDirectionFlag::TOP;
			}

			if (y != 0) {
				otherBlock.setChunkBlock(&_blocks[i - Chunk::LAYER_SIZE],
					i - Chunk::LAYER_SIZE, this);
			}
			else if (_adjacentChunks[(uint8_t)FaceDirection::BOTTOM] != nullptr) {
				otherBlock.setChunkBlock(&_adjacentChunks[(uint8_t)FaceDirection::BOTTOM]->_blocks[i + Chunk::BLOCK_COUNT - Chunk::LAYER_SIZE], i + Chunk::BLOCK_COUNT - Chunk::LAYER_SIZE, _adjacentChunks[(uint8_t)FaceDirection::BOTTOM]);
			}

			if (y == 0 && _adjacentChunks[(uint8_t)FaceDirection::BOTTOM] == nullptr || otherBlock.isEmpty() ||
				(!otherBlock.isSolid() &&
					(!otherBlock.isFaceFull(FaceDirection::TOP) ||
						otherBlock.isFaceTransparent(FaceDirection::TOP) &&
						otherBlock.getId() != curBlock.getId()))) {
				faceNum = (uint8_t)FaceDirection::BOTTOM;
				const BlockFace* face = curBlock.getBlockFace(FaceDirection::BOTTOM);
				_vertexCount += face->vertexCount;
				_indexCount[faceNum] += face->indexCount;
				visableFaces[i] |= FaceDirectionFlag::BOTTOM;
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

	// Allocate Mesh Buffers
	if (_vertexCount == 0) {
		_vertexBuffer = (float*)Chunk::s_emptyBuffer;
	}
	else {
		uint64_t count = (uint64_t)_vertexCount * 5 * sizeof(float);
		_vertexBuffer = (float*)malloc((uint64_t)_vertexCount * 5 * sizeof(float));
	}

	_vertexCount = 0;

	for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
		if (_indexCount[i] == 0) {
			_buffers[i] = (uint32_t*)Chunk::s_emptyBuffer;
		}
		else {
			_buffers[i] = (uint32_t*)malloc(_indexCount[i] * sizeof(uint32_t));
		}

		_indexCount[i] = 0;
	}

	// Create The Mesh
	x = 0;
	y = 0;
	z = 0;
	for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
		curBlock.setChunkBlock(&_blocks[i], i);
		for (uint8_t faceNum = 0; faceNum < (uint8_t)FaceDirection::FACECOUNT;
			++faceNum) {
			if (visableFaces[i] & FaceDirectionFlag::DirectionToFlag(faceNum)) {
				const BlockFace* face = curBlock.getBlockFace((FaceDirection)faceNum);

				for (uint32_t j = 0; j < face->indexCount; ++j) {
					_buffers[faceNum][_indexCount[faceNum] + j] =
						(face->indices[j]) + _vertexCount;
				}

				for (uint16_t j = 0; j < face->vertexCount; ++j) {
					_vertexBuffer[_vertexCount * 5] = face->vertices[j * 3] + x;
					_vertexBuffer[_vertexCount * 5 + 1] = face->vertices[j * 3 + 1] + y;
					_vertexBuffer[_vertexCount * 5 + 2] = face->vertices[j * 3 + 2] - z;
					_vertexBuffer[_vertexCount * 5 + 3] = face->uvs[j * 2];
					_vertexBuffer[_vertexCount * 5 + 4] = face->uvs[j * 2 + 1];
					++_vertexCount;
				}

				_indexCount[faceNum] += face->indexCount;
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

	_buffersDirty = true;
}

void Chunk::updateAdjacentChunks(ChunkManager* mgr) {
	Chunk* ch =
		mgr->GetChunk(_position + FaceDirectionFlag::DirectionToInt32Vector(
			FaceDirection::LEFT));
	if (ch != nullptr) {
		ch->grab();
		{
			std::lock_guard<std::mutex> lock(_meshBuffersLock);
			if (_adjacentChunks[(uint8_t)FaceDirection::LEFT] == nullptr) {
				ch->grab();
				_adjacentChunks[(uint8_t)FaceDirection::LEFT] = ch;
				_chunkModified = true;
			}

			assert(ch == _adjacentChunks[(uint8_t)FaceDirection::LEFT]);
		}

		{
			std::lock_guard<std::mutex> lock(ch->_meshBuffersLock);
			if (ch->_adjacentChunks[(uint8_t)FaceDirection::RIGHT] == nullptr) {
				grab();
				ch->_adjacentChunks[(uint8_t)FaceDirection::RIGHT] = this;
				ch->_chunkModified = true;
			}

			assert(ch->_adjacentChunks[(uint8_t)FaceDirection::RIGHT] == this);
		}

		ch->drop();
	}

	ch = mgr->GetChunk(_position + FaceDirectionFlag::DirectionToInt32Vector(
		FaceDirection::RIGHT));
	if (ch != nullptr) {
		ch->grab();
		{
			std::lock_guard<std::mutex> lock(_meshBuffersLock);
			if (_adjacentChunks[(uint8_t)FaceDirection::RIGHT] == nullptr) {
				ch->grab();
				_adjacentChunks[(uint8_t)FaceDirection::RIGHT] = ch;
				_chunkModified = true;
			}

			assert(ch == _adjacentChunks[(uint8_t)FaceDirection::RIGHT]);
		}

		{
			std::lock_guard<std::mutex> lock(ch->_meshBuffersLock);
			if (ch->_adjacentChunks[(uint8_t)FaceDirection::LEFT] == nullptr) {
				grab();
				ch->_adjacentChunks[(uint8_t)FaceDirection::LEFT] = this;
				ch->_chunkModified = true;
			}

			assert(ch->_adjacentChunks[(uint8_t)FaceDirection::LEFT] == this);
		}

		ch->drop();
	}

	ch = mgr->GetChunk(_position + FaceDirectionFlag::DirectionToInt32Vector(
		FaceDirection::TOP));
	if (ch != nullptr) {
		ch->grab();
		{
			std::lock_guard<std::mutex> lock(_meshBuffersLock);
			if (_adjacentChunks[(uint8_t)FaceDirection::TOP] == nullptr) {
				ch->grab();
				_adjacentChunks[(uint8_t)FaceDirection::TOP] = ch;
				_chunkModified = true;
			}

			assert(ch == _adjacentChunks[(uint8_t)FaceDirection::TOP]);
		}

		{
			std::lock_guard<std::mutex> lock(ch->_meshBuffersLock);
			if (ch->_adjacentChunks[(uint8_t)FaceDirection::BOTTOM] == nullptr) {
				grab();
				ch->_adjacentChunks[(uint8_t)FaceDirection::BOTTOM] = this;
				ch->_chunkModified = true;
			}

			assert(ch->_adjacentChunks[(uint8_t)FaceDirection::BOTTOM] == this);
		}

		ch->drop();
	}

	ch = mgr->GetChunk(_position + FaceDirectionFlag::DirectionToInt32Vector(
		FaceDirection::BOTTOM));
	if (ch != nullptr) {
		ch->grab();
		{
			std::lock_guard<std::mutex> lock(_meshBuffersLock);
			if (_adjacentChunks[(uint8_t)FaceDirection::BOTTOM] == nullptr) {
				ch->grab();
				_adjacentChunks[(uint8_t)FaceDirection::BOTTOM] = ch;
				_chunkModified = true;
			}

			assert(ch == _adjacentChunks[(uint8_t)FaceDirection::BOTTOM]);
		}

		{
			std::lock_guard<std::mutex> lock(ch->_meshBuffersLock);
			if (ch->_adjacentChunks[(uint8_t)FaceDirection::TOP] == nullptr) {
				grab();
				ch->_adjacentChunks[(uint8_t)FaceDirection::TOP] = this;
				ch->_chunkModified = true;
			}

			assert(ch->_adjacentChunks[(uint8_t)FaceDirection::TOP] == this);
		}

		ch->drop();
	}

	ch = mgr->GetChunk(_position + FaceDirectionFlag::DirectionToInt32Vector(
		FaceDirection::FRONT));
	if (ch != nullptr) {
		ch->grab();
		{
			std::lock_guard<std::mutex> lock(_meshBuffersLock);
			if (_adjacentChunks[(uint8_t)FaceDirection::FRONT] == nullptr) {
				ch->grab();
				_adjacentChunks[(uint8_t)FaceDirection::FRONT] = ch;
				_chunkModified = true;
			}

			assert(ch == _adjacentChunks[(uint8_t)FaceDirection::FRONT]);
		}

		{
			std::lock_guard<std::mutex> lock(ch->_meshBuffersLock);
			if (ch->_adjacentChunks[(uint8_t)FaceDirection::BACK] == nullptr) {
				grab();
				ch->_adjacentChunks[(uint8_t)FaceDirection::BACK] = this;
				ch->_chunkModified = true;
			}

			assert(ch->_adjacentChunks[(uint8_t)FaceDirection::BACK] == this);
		}

		ch->drop();
	}

	ch = mgr->GetChunk(_position + FaceDirectionFlag::DirectionToInt32Vector(
		FaceDirection::BACK));
	if (ch != nullptr) {
		ch->grab();
		{
			std::lock_guard<std::mutex> lock(_meshBuffersLock);
			if (_adjacentChunks[(uint8_t)FaceDirection::BACK] == nullptr) {
				ch->grab();
				_adjacentChunks[(uint8_t)FaceDirection::BACK] = ch;
				_chunkModified = true;
			}

			assert(ch == _adjacentChunks[(uint8_t)FaceDirection::BACK]);
		}

		{
			std::lock_guard<std::mutex> lock(ch->_meshBuffersLock);
			if (ch->_adjacentChunks[(uint8_t)FaceDirection::FRONT] == nullptr) {
				grab();
				ch->_adjacentChunks[(uint8_t)FaceDirection::FRONT] = this;
				ch->_chunkModified = true;
			}

			assert(ch->_adjacentChunks[(uint8_t)FaceDirection::FRONT] == this);
		}

		ch->drop();
	}
}

void Chunk::updateBuffers() {
	if (_meshBuffersLock.try_lock()) {
		if (_vertexBuffer != nullptr) {
			glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
			glBufferData(GL_ARRAY_BUFFER, _vertexCount * 5 * sizeof(float),
				_vertexBuffer, GL_STATIC_DRAW);
			if (_vertexBuffer != Chunk::s_emptyBuffer) free(_vertexBuffer);
			_vertexBuffer = nullptr;
		}

		for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
			if (_buffers[i] != nullptr) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount[i] * sizeof(uint32_t),
					_buffers[i], GL_STATIC_DRAW);
				_currentIndexCount[i] = _indexCount[i];
				_indexCount[i] = 0;
				if (_buffers[i] != Chunk::s_emptyBuffer) free(_buffers[i]);
				_buffers[i] = nullptr;
			}
		}

		_buffersDirty = false;
		_meshBuffersLock.unlock();
	}
}
