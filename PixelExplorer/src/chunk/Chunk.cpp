#include "Chunk.h"

#include <chunk/block/Block.h>
#include <iostream>

void* Chunk::s_emptyBuffer = malloc(1);

Chunk::Chunk() : Renderable(Material::getDefault()) {
	setPosition(glm::vec3(0));
	_visible = true;
	_vertexBuffer = nullptr;
	_vertexCount = 0;
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

	//for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
	//  
	//}
}

Chunk::~Chunk() {}

bool Chunk::onPreRender(float deltaTime, float* cameraPos,
	float* cameraRotation) {
	if (_buffersDirty) updateBuffers();
	_visibleFaces = FaceDirectionFlag::FRONT;
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
	std::lock_guard<std::mutex> lock(_meshBuffersLock);

	// Clear Old Buffer Data
	if (_vertexBuffer != nullptr) {
		if (_vertexBuffer != Chunk::s_emptyBuffer)
			free(_vertexBuffer);
		_vertexBuffer = nullptr;
	}

	_vertexCount = 0;

	for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
		if (_buffers[i] != nullptr) {
			if (_buffers[i] != Chunk::s_emptyBuffer)
				free(_buffers[i]);
			_buffers[i] = nullptr;
		}

		_indexCount[i] = 0;
	}

	// uint8_t x = 0;
	// uint8_t y = 0;
	// uint8_t z = 0;
	Block curBlock(this);
	uint8_t faceNum;
	for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
		curBlock.setChunkBlock(&_blocks[i], i);
		// Front Faces
		faceNum = (uint8_t)FaceDirection::FRONT;
		const BlockFace* face = curBlock.getBlockFace(FaceDirection::FRONT);
		_vertexCount += face->vertexCount;
		_indexCount[faceNum] += face->indexCount;

		// if (++x == Chunk::CHUNK_SIZE) {
		//  x = 0;
		//  if (++z == Chunk::CHUNK_SIZE) {
		//    z = 0;
		//    ++y;
		//  }
		//}
	}

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

	// x = 0;
	// y = 0;
	// z = 0;
	for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
		curBlock.setChunkBlock(&_blocks[i], i);
		// Front Faces
		faceNum = (uint8_t)FaceDirection::FRONT;
		const BlockFace* face = curBlock.getBlockFace(FaceDirection::FRONT);
		for (uint16_t j = 0; j < face->vertexCount; ++j) {
			_vertexBuffer[_vertexCount * 5] = face->vertices[j * 3];
			_vertexBuffer[_vertexCount * 5 + 1] = face->vertices[j * 3 + 1];
			_vertexBuffer[_vertexCount * 5 + 2] = face->vertices[j * 3 + 2];
			_vertexBuffer[_vertexCount * 5 + 3] = face->uvs[j * 2];
			_vertexBuffer[_vertexCount * 5 + 4] = face->uvs[j * 2 + 1];
			++_vertexCount;
		}

		for (uint32_t j = 0; j < face->indexCount; ++j)
			_buffers[faceNum][_indexCount[faceNum] + j] = (face->indices[j]) + _indexCount[faceNum];

		_indexCount[faceNum] += face->indexCount;

		// if (++x == Chunk::CHUNK_SIZE) {
		//  x = 0;
		//  if (++z == Chunk::CHUNK_SIZE) {
		//    z = 0;
		//    ++y;
		//  }
		//}
	}

	_buffersDirty = true;
}

void Chunk::updateBuffers() {
	if (_meshBuffersLock.try_lock()) {
		if (_vertexBuffer != nullptr) {
			glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
			glBufferData(GL_ARRAY_BUFFER, (uint64_t)_vertexCount,
				(void*)_vertexBuffer, GL_STATIC_DRAW);
			if (_vertexBuffer != Chunk::s_emptyBuffer)
			  free(_vertexBuffer);
			_vertexBuffer = nullptr;
		}

		for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
			if (_buffers[i] != nullptr) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					_indexCount[i] * sizeof(unsigned short), _buffers[i],
					GL_STATIC_DRAW);
				_currentIndexCount[i] = _indexCount[i];
				_indexCount[i] = 0;
				if (_buffers[i] != Chunk::s_emptyBuffer)
				  free(_buffers[i]);
				_buffers[i] = nullptr;
			}
		}

		_buffersDirty = false;
		_meshBuffersLock.unlock();
	}
}
