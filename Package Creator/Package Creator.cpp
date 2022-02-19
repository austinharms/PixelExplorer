#include <fstream>
#include <iostream>
#include <string>

int main() {
  //std::cout << "Creating Package, Enter Package Name:" << std::endl;
  //std::string name;
  //std::cin >> name;
  char end = 0;
  std::ofstream package("./blocks.module", std::ios::binary);

  uint16_t version = 1;
  package.write((const char*)&version, sizeof(uint16_t));
  std::cout << "Version: " << version << std::endl;

  uint16_t blockCount;
  std::cout << "Enter Block Count:" << std::endl;
  std::cin >> blockCount;
  package.write((const char*)&blockCount, sizeof(uint16_t));
  for (uint16_t block = 0; block < blockCount; ++block) {
    std::string name;
    std::cout << "Enter Name:" << std::endl;
    std::cin >> name;
    package.write(name.c_str(), name.length() * sizeof(char));
    package.write(&end, sizeof(char));
  }

  for (uint16_t block = 0; block < blockCount; ++block) {
    uint8_t solid = 1;
    package.write((const char*)&solid, sizeof(uint8_t));

    // Front Face
    {
      float vertices[12] = {
          -0.5f, -0.5f, 0.5f, 0.5f,  -0.5f, 0.5f,
          0.5f,  0.5f,  0.5f, -0.5f, 0.5f,  0.5f,

      };

      float uvs[12] = {0, 0, 1, 0, 1, 1, 0, 1};

      uint8_t indices[6] = {0, 1, 2, 0, 2, 3};

      uint8_t face = 0;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      package.write((const char*)&vertices, sizeof(float) * vertexCount * 3);
      package.write((const char*)&uvs, sizeof(float) * vertexCount * 2);
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      package.write((const char*)&indices, sizeof(uint8_t) * indexCount);
    }

    // Back Face
    {
      float vertices[12] = {
          -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,
          0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,

      };

      float uvs[12] = {1, 0, 0, 0, 0, 1, 1, 1};

      uint8_t indices[6] = {0, 3, 1, 1, 3, 2};

      uint8_t face = 1;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      package.write((const char*)&vertices, sizeof(float) * vertexCount * 3);
      package.write((const char*)&uvs, sizeof(float) * vertexCount * 2);
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      package.write((const char*)&indices, sizeof(uint8_t) * indexCount);
    }

    // Left Face
    {
      float vertices[12] = {
          -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5,
      };

      float uvs[12] = {0, 0, 1, 0, 1, 1, 0, 1};

      uint8_t indices[6] = {0, 1, 2, 0, 2, 3};

      uint8_t face = 2;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      package.write((const char*)&vertices, sizeof(float) * vertexCount * 3);
      package.write((const char*)&uvs, sizeof(float) * vertexCount * 2);
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      package.write((const char*)&indices, sizeof(uint8_t) * indexCount);
    }

    // Right Face
    {
      float vertices[12] = {
          0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
      };

      float uvs[12] = {1, 0, 0, 0, 0, 1, 1, 1};

      uint8_t indices[6] = {0, 3, 1, 1, 3, 2};

      uint8_t face = 3;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      package.write((const char*)&vertices, sizeof(float) * vertexCount * 3);
      package.write((const char*)&uvs, sizeof(float) * vertexCount * 2);
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      package.write((const char*)&indices, sizeof(uint8_t) * indexCount);
    }

    // Top Face
    {
      float vertices[12] = {
          -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5,
      };

      float uvs[12] = {0, 1, 1, 1, 1, 0, 0, 0};

      uint8_t indices[6] = {0, 3, 1, 1, 3, 2};

      uint8_t face = 4;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      package.write((const char*)&vertices, sizeof(float) * vertexCount * 3);
      package.write((const char*)&uvs, sizeof(float) * vertexCount * 2);
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      package.write((const char*)&indices, sizeof(uint8_t) * indexCount);
    }

    // Bottom Face
    {
      float vertices[12] = {
          -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5,
      };

      float uvs[12] = {0, 0, 1, 0, 1, 1, 0, 1};

      uint8_t indices[6] = {0, 1, 2, 0, 2, 3};

      uint8_t face = 5;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      package.write((const char*)&vertices, sizeof(float) * vertexCount * 3);
      package.write((const char*)&uvs, sizeof(float) * vertexCount * 2);
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      package.write((const char*)&indices, sizeof(uint8_t) * indexCount);
    }
  }

  package.write(&end, sizeof(char));
  package.close();
  std::cout << "Created Block Module" << std::endl;
}