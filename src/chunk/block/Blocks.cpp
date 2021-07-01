#include "Blocks.h"

#include <fstream>

std::vector<BlockBase*>* Blocks::s_blocksArray = nullptr;
BlockBase* Blocks::s_defaultBlock = nullptr;
std::recursive_mutex Blocks::s_blocksLock;

Blocks::Blocks() {}

Blocks::~Blocks() {}

void Blocks::loadBlocks(const char* blockDataPath) {
  std::lock_guard<std::recursive_mutex> lock(Blocks::s_blocksLock);
  Blocks::dropBlocks();
  Blocks::s_blocksArray = new std::vector<BlockBase*>();
  if (blockDataPath == nullptr) return;
  std::ifstream blocksFile;
  blocksFile.open(blockDataPath, std::ios::binary);
  int32_t length;
  blocksFile.read((char*)&length, sizeof(int32_t));
  s_blocksArray->reserve(length);

  char boolVal;
  float floatVal;
  bool transparent;
  uint32_t id;
  unsigned char charVal = 0;
  for (int32_t i = 0; i < length; ++i) {
    blocksFile.read((char*)&id, sizeof(uint32_t));
    if (id == 0) continue;
    blocksFile.read(&boolVal, 1);
    transparent = boolVal == 1;
    BlockBase::BlockFace* blockFace = new BlockBase::BlockFace[6];
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
    BlockBase* block = new BlockBase(id, transparent, blockFace);
    addBlock(block, id);
    block->drop();
  }
  blocksFile.close();
}

void Blocks::addBlock(BlockBase* block, int32_t id) {
  if (id == 0) return;
  std::lock_guard<std::recursive_mutex> lock(Blocks::s_blocksLock);
  block->grab();
  size_t size = s_blocksArray->size();
  if (size > id) {
    if ((*s_blocksArray)[id] == nullptr) {
      (*s_blocksArray)[id] = block;
    } else {
      block->drop();
    }
  } else if (s_blocksArray->size() == id) {
    s_blocksArray->emplace_back(block);
  } else {
    do {
      s_blocksArray->emplace_back(nullptr);
    } while (s_blocksArray->size() < id);
    s_blocksArray->emplace_back(block);
  }
}

int32_t Blocks::addBlock(BlockBase* block) {
  std::lock_guard<std::recursive_mutex> lock(Blocks::s_blocksLock);
  block->grab();
  s_blocksArray->emplace_back(block);
  return s_blocksArray->size() - 1;
}

BlockBase* Blocks::getBlock(int32_t id) {
  std::lock_guard<std::recursive_mutex> lock(Blocks::s_blocksLock);
  if (s_blocksArray != nullptr && id < s_blocksArray->size()) {
    BlockBase* block = (*s_blocksArray)[id];
    if (block != nullptr) return block;
  }

  return s_defaultBlock;
}

void Blocks::saveBlocks(const char* blockDataPath) {
  std::lock_guard<std::recursive_mutex> lock(Blocks::s_blocksLock);
  if (Blocks::s_blocksArray != nullptr) {
    std::ofstream blocksFile;
    blocksFile.open(blockDataPath, std::ios::binary);
    int32_t length = s_blocksArray->size();
    blocksFile.write((char*)&length, sizeof(int32_t));
    char boolVal = 0;
    int32_t zero = 0;
    int32_t id = 0;
    for (const BlockBase* block : *Blocks::s_blocksArray) {
      if (block == nullptr) {
        blocksFile.write((char*)&zero, sizeof(int32_t));
        continue;
      }
      id = block->getID();
      blocksFile.write((char*)(&id), sizeof(int32_t));
      boolVal = block->getTransparent() ? 1 : 0;
      blocksFile.write((char*)(&(boolVal)), 1);
      for (unsigned char i = 0; i < 6; ++i) {
        BlockBase::BlockFace* f = block->getBlockFace((BlockBase::Face)i);
        boolVal = f->fullFace ? 1 : 0;
        blocksFile.write((char*)(&(boolVal)), 1);
        blocksFile.write((char*)(&(f->vertexCount)), sizeof(unsigned char));
        for (unsigned char vert = 0; vert < f->vertexCount; ++vert) {
          blocksFile.write((char*)(&(f->vertices[vert * 3])), sizeof(float));
          blocksFile.write((char*)(&(f->vertices[vert * 3 + 1])),
                           sizeof(float));
          blocksFile.write((char*)(&(f->vertices[vert * 3 + 2])),
                           sizeof(float));
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

void Blocks::dropBlocks() {
  std::lock_guard<std::recursive_mutex> lock(Blocks::s_blocksLock);
  if (s_defaultBlock != nullptr) s_defaultBlock->drop();
  s_defaultBlock = nullptr;
  if (s_blocksArray != nullptr) {
    for (BlockBase*& block : *s_blocksArray) {
      if (block != nullptr) block->drop();
    }

    delete s_blocksArray;
    s_blocksArray = nullptr;
  }
}

void Blocks::setDefaultBlock(BlockBase* block) {
  block->grab();
  if (s_defaultBlock != nullptr) s_defaultBlock->drop();
  s_defaultBlock = block;
}

/*
  float tw = (1.0f / 96.0f) * 16.0f;
  float th = (1.0f / 64.0f) * 16.0f;
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