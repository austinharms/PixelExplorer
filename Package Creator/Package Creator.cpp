#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::cout << "Creating Package, Enter Package Name:" << std::endl;
  std::string name;
  std::cin >> name;

  std::ofstream package((std::string("./") + name + ".pxb").c_str(),
                        std::ios::binary);

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
    uint8_t nameLength = name.length();
    package.write((const char*)&nameLength, sizeof(uint8_t));
    package.write(name.c_str(), nameLength);
    uint8_t solid = 1;
    package.write((const char*)&solid, sizeof(uint8_t));
    uint8_t faceCount = 6;
    package.write((const char*)&faceCount, sizeof(uint8_t));

    // Front Face
    {
      float vertices[12] = {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

      };

      float uvs[12] = {
        0,0,
        1,0,
        1,1,
        0,1
      };

      uint8_t indices[6] = {
        0, 1, 2, 0, 2, 3
      };

      uint8_t face = 0;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      for (uint8_t vertex = 0; vertex < vertexCount; ++vertex) {
        for (uint8_t i = 0; i < 3; ++i) {
          float vert = vertices[vertex * 3 + i];
          package.write((const char*)&vert, sizeof(float));
        }

        for (uint8_t i = 0; i < 2; ++i) {
          float vert = uvs[vertex * 2 + i];
          package.write((const char*)&vert, sizeof(float));
        }
      }
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      for (uint8_t index = 0; index < indexCount; ++index) {
        uint8_t ind = indices[index];
        package.write((const char*)&ind, sizeof(uint8_t));
      }
    }

    // Back Face
    {
      float vertices[12] = {
          -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f,
          0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f,

      };

      float uvs[12] = {0, 0, 1, 0, 1, 1, 0, 1};

      uint8_t indices[6] = {0, 3, 2, 0, 2, 1};

      uint8_t face = 1;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      for (uint8_t vertex = 0; vertex < vertexCount; ++vertex) {
        for (uint8_t i = 0; i < 3; ++i) {
          float vert = vertices[vertex * 3 + i];
          package.write((const char*)&vert, sizeof(float));
        }

        for (uint8_t i = 0; i < 2; ++i) {
          float vert = uvs[vertex * 2 + i];
          package.write((const char*)&vert, sizeof(float));
        }
      }
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      for (uint8_t index = 0; index < indexCount; ++index) {
        uint8_t ind = indices[index];
        package.write((const char*)&ind, sizeof(uint8_t));
      }
    }


    // Left Face
    {
      float vertices[12] = {
        -0.5, -0.5, -0.5,
        -0.5, -0.5,  0.5,
        -0.5,  0.5,  0.5,
        -0.5,  0.5, -0.5,
      };

      float uvs[12] = {
        0,0,
        1,0,
        1,1,
        0,1
      };

      uint8_t indices[6] = {
        0, 1, 2, 0, 2, 3
      };

      uint8_t face = 2;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      for (uint8_t vertex = 0; vertex < vertexCount; ++vertex) {
        for (uint8_t i = 0; i < 3; ++i) {
          float vert = vertices[vertex * 3 + i];
          package.write((const char*)&vert, sizeof(float));
        }

        for (uint8_t i = 0; i < 2; ++i) {
          float vert = uvs[vertex * 2 + i];
          package.write((const char*)&vert, sizeof(float));
        }
      }
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      for (uint8_t index = 0; index < indexCount; ++index) {
        uint8_t ind = indices[index];
        package.write((const char*)&ind, sizeof(uint8_t));
      }
    }

    // Right Face
    {
      float vertices[12] = {
         0.5, -0.5, -0.5,
         0.5, -0.5,  0.5,
         0.5,  0.5,  0.5,
         0.5,  0.5, -0.5,
      };

      float uvs[12] = {
        0,0,
        1,0,
        1,1,
        0,1
      };

      uint8_t indices[6] = {
        0, 3, 2, 0, 2, 1
      };

      uint8_t face = 3;
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 4;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      for (uint8_t vertex = 0; vertex < vertexCount; ++vertex) {
        for (uint8_t i = 0; i < 3; ++i) {
          float vert = vertices[vertex * 3 + i];
          package.write((const char*)&vert, sizeof(float));
        }

        for (uint8_t i = 0; i < 2; ++i) {
          float vert = uvs[vertex * 2 + i];
          package.write((const char*)&vert, sizeof(float));
        }
      }
      uint8_t indexCount = 6;
      package.write((const char*)&indexCount, sizeof(uint8_t));
      for (uint8_t index = 0; index < indexCount; ++index) {
        uint8_t ind = indices[index];
        package.write((const char*)&ind, sizeof(uint8_t));
      }
    }


    for (uint8_t face = 4; face < 6; ++face) {
      package.write((const char*)&face, sizeof(uint8_t));
      uint8_t faceSolid = 1;
      package.write((const char*)&faceSolid, sizeof(uint8_t));
      uint8_t faceTransparent = 0;
      package.write((const char*)&faceTransparent, sizeof(uint8_t));
      uint8_t vertexCount = 0;
      package.write((const char*)&vertexCount, sizeof(uint8_t));
      uint8_t indexCount = 0;
      package.write((const char*)&indexCount, sizeof(uint8_t));
    }
  }

  char end = '\0';
  package.write(&end, 1);
  package.close();
  std::cout << "Created Package" << std::endl;
}