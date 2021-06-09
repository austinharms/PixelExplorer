#include "Block.h"

#include <fstream>
#include <iostream>
#include <unordered_map>

std::unordered_map<uint32_t, Block*>* Block::s_blocks = nullptr;
Block* Block::s_defaultBlock = nullptr;

Block::Block(uint32_t id, bool transparent, BlockFace* blockFaces)
    : _id(id), _transparent(transparent), _faces(blockFaces) {}

Block::~Block() { delete[] _faces; }

void Block::loadBlocks(const char* blockDataPath) {
  Block::s_blocks = new std::unordered_map<uint32_t, Block*>();
  std::ifstream blocksFile;
  blocksFile.open(blockDataPath, std::ios::binary);
  uint32_t length;
  blocksFile.read((char*)&length, sizeof(uint32_t));
  char boolVal;
  float floatVal;
  bool transparent;
  uint32_t id;
  unsigned char charVal = 0;
  for (uint32_t i = 0; i < length; ++i) {
    blocksFile.read((char*)&id, sizeof(uint32_t));
    blocksFile.read(&boolVal, 1);
    transparent = boolVal == 1;
    BlockFace* blockFace = new BlockFace[6];
    for (unsigned char i = 0; i < 6; ++i) {
      blocksFile.read(&boolVal, 1);
      blockFace[i].fullFace = boolVal == 1;
      blocksFile.read((char*)&charVal, sizeof(unsigned char));
      blockFace[i].vertexCount = charVal;
      blockFace[i].uvs = new float[charVal * 2];
      blockFace[i].vertices = new float[charVal * 3];
      for (unsigned char vert = 0; vert < blockFace[i].vertexCount; ++vert) {
        blocksFile.read((char*)&floatVal, sizeof(float));
        blockFace[i].vertices[vert * 3] = floatVal;
        blocksFile.read((char*)&floatVal, sizeof(float));
        blockFace[i].vertices[vert * 3 + 1] = floatVal;
        blocksFile.read((char*)&floatVal, sizeof(float));
        blockFace[i].vertices[vert * 3 + 2] = floatVal;
        blocksFile.read((char*)&floatVal, sizeof(float));
        blockFace[i].uvs[vert * 2] = floatVal;
        blocksFile.read((char*)&floatVal, sizeof(float));
        blockFace[i].uvs[vert * 2 + 1] = floatVal;
      }

      blocksFile.read((char*)&charVal, sizeof(unsigned char));
      blockFace[i].indexCount = charVal;
      blockFace[i].indices = new unsigned char[charVal];
      for (unsigned char in = 0; in < blockFace[i].indexCount; ++in) {
        blocksFile.read((char*)&charVal, sizeof(unsigned char));
        blockFace[i].indices[in] = charVal;
      }
    }
    Block* block = new Block(id, transparent, blockFace);
    addBlock(block);
    block->drop();
  }
  blocksFile.close();
}

Block* Block::getBlock(uint32_t id) {
  auto block = Block::s_blocks->find(id);
  if (block != Block::s_blocks->end()) {
    return block->second;
  } else {
    return s_defaultBlock;
  }
}

bool Block::addBlock(Block* block) {
  block->grab();
  return Block::s_blocks->insert({block->_id, block}).second;
}

void Block::saveBlocks(const char* blockDataPath) {
  if (Block::s_blocks != nullptr) {
    std::ofstream blocksFile;
    blocksFile.open(blockDataPath, std::ios::binary);
    uint32_t length = Block::s_blocks->size();
    blocksFile.write((char*)&length, sizeof(uint32_t));
    char boolVal = 0;
    for (const std::pair<uint32_t, Block*>& block : *Block::s_blocks) {
      blocksFile.write((char*)(&(block.first)), sizeof(uint32_t));
      boolVal = block.second->_transparent ? 1 : 0;
      blocksFile.write((char*)(&(boolVal)), 1);
      for (unsigned char i = 0; i < 6; ++i) {
        BlockFace* f = block.second->getBlockFace((Face)i);
        boolVal = f->fullFace ? 1 : 0;
        blocksFile.write((char*)(&(boolVal)), 1);
        blocksFile.write((char*)(&(f->vertexCount)), sizeof(unsigned char));
        for (unsigned char vert = 0; vert < f->vertexCount; ++vert) {
          blocksFile.write((char*)(&(f->vertices[vert * 3])), sizeof(float));
          blocksFile.write((char*)(&(f->vertices[vert * 3 + 1])), sizeof(float));
          blocksFile.write((char*)(&(f->vertices[vert * 3 + 2])), sizeof(float));
          blocksFile.write((char*)(&(f->uvs[vert * 2])), sizeof(float));
          blocksFile.write((char*)(&(f->uvs[vert * 2 + 1])), sizeof(float));
        }
        blocksFile.write((char*)(&(f->indexCount)), sizeof(unsigned char));
        for (unsigned char in = 0; in < f->indexCount; ++in) {
          blocksFile.write((char*)(&(f->indices[in])), sizeof(unsigned char));
        }
      }
    }
    blocksFile.close();
  }
}

void Block::dropBlocks() {
  if (s_defaultBlock != nullptr) s_defaultBlock->drop();
  if (Block::s_blocks != nullptr) {
    for (const std::pair<uint32_t, Block*>& block : *Block::s_blocks)
      block.second->drop();
    delete Block::s_blocks;
  }
}


/*
  Block::BlockFace* faces = new Block::BlockFace[6];
  faces[Block::FRONT].indexCount = 6;
  faces[Block::FRONT].vertexCount = 4;
  faces[Block::FRONT].vertices = new float[12]{
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f
  };
  faces[Block::FRONT].indices = new unsigned char[6]{0, 3, 2, 2, 1, 0};
  faces[Block::FRONT].uvs = new float[8]{
    tw * 2, th * 1,
    tw * 2, th * 0,
    tw * 3, th * 0,
    tw * 3, th * 1,
  };

  faces[Block::BACK].indexCount = 6;
  faces[Block::BACK].vertexCount = 4;
  faces[Block::BACK].vertices = new float[12]{
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f
  };
  faces[Block::BACK].indices = new unsigned char[6]{2, 3, 0, 0, 1, 2};
  faces[Block::BACK].uvs = new float[8]{
    tw * 4, th * 1,
    tw * 4, th * 0,
    tw * 3, th * 0,
    tw * 3, th * 1,
  };

  faces[Block::LEFT].indexCount = 6;
  faces[Block::LEFT].vertexCount = 4;
  faces[Block::LEFT].vertices = new float[12]{
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f
  };
  faces[Block::LEFT].indices = new unsigned char[6]{0, 3, 2, 2, 1, 0};
  faces[Block::LEFT].uvs = new float[8]{
    tw * 4, th * 1,
    tw * 4, th * 0,
    tw * 5, th * 0,
    tw * 5, th * 1,
  };


  faces[Block::RIGHT].indexCount = 6;
  faces[Block::RIGHT].vertexCount = 4;
  faces[Block::RIGHT].vertices = new float[12]{
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f
  };
  faces[Block::RIGHT].indices = new unsigned char[6]{2, 3, 0, 0, 1, 2};
  faces[Block::RIGHT].uvs = new float[8]{
    tw * 6, th * 1,
    tw * 6, th * 0,
    tw * 5, th * 0,
    tw * 5, th * 1,
  };

  faces[Block::TOP].indexCount = 6;
  faces[Block::TOP].vertexCount = 4;
  faces[Block::TOP].vertices = new float[12]{
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f
  };
  faces[Block::TOP].indices = new unsigned char[6]{2, 3, 0, 0, 1, 2};
  faces[Block::TOP].uvs = new float[8]{
    tw * 0, th * 0,
    tw * 0, th * 1,
    tw * 1, th * 1,
    tw * 1, th * 0,
  };

  faces[Block::BOTTOM].indexCount = 6;
  faces[Block::BOTTOM].vertexCount = 4;
  faces[Block::BOTTOM].vertices = new float[12]{
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f
  };
  faces[Block::BOTTOM].indices = new unsigned char[6]{0, 3, 2, 2, 1, 0};
  faces[Block::BOTTOM].uvs = new float[8]{
    tw * 1, th * 0,
    tw * 1, th * 1,
    tw * 2, th * 1,
    tw * 2, th * 0,
  };
    Block* blockPtr = new Block(0, false, faces);
    Block::addBlock(blockPtr);
      blockPtr->drop();
    
    */