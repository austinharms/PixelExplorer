#include "Shape.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_map>

#include "Logger.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace pixelexplorer::game::block {
	Shape::Shape(const std::string& path) : name(getFileName(path))
	{
		indices = nullptr;
		vertices = nullptr;
		vertexCount = 0;
		indexCount = 0;

		FILE* shapeFile;
		if (fopen_s(&shapeFile, path.c_str(), "r") != 0 || shapeFile == nullptr) {
			Logger::error("Failed to open shape file " + path);
			return;
		}

		std::vector<uint8_t> fileIndices;
		std::vector<Vec5> fullVertices;

		{
			std::vector<glm::vec3> fileVertices;
			std::vector<glm::vec2> fileUVs;
			char lineStart[3];
			lineStart[2] = 0;

			while (fread(lineStart, 1, 2, shapeFile) == 2)
			{
				if (strcmp(lineStart, "v ") == 0) {
					glm::vec3 v;
					fscanf_s(shapeFile, "%f %f %f\n", &v.x, &v.y, &v.z);
					fileVertices.push_back(v);
				}
				else if (strcmp(lineStart, "vt") == 0) {
					glm::vec2 v;
					fscanf_s(shapeFile, "%f %f\n", &v.x, &v.y);
					fileUVs.push_back(v);
				}
				else if (strcmp(lineStart, "f ") == 0) {
					int32_t vertexIndex[3], uvIndex[3], normalIndex[3];
					if (fscanf_s(shapeFile, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]) != 9) {
						Logger::error("Failed to parse shape file " + path);
						return;
					}

					for (uint8_t i = 0; i < 3; ++i) {
						//if (indexCount == 0xff) {
						//	Logger::error("Failed to load shape, Max of 255 indices " + path);
						//	fclose(shapeFile);
						//	return;
						//}

						Vec5 point;
						point.x = fileVertices[vertexIndex[i] - 1].x;
						point.y = fileVertices[vertexIndex[i] - 1].y;
						point.z = fileVertices[vertexIndex[i] - 1].z;
						point.u = fileUVs[uvIndex[i] - 1].x;
						point.v = fileUVs[uvIndex[i] - 1].y;

						auto it = std::find(fullVertices.begin(), fullVertices.end(), point);
						if (it == fullVertices.end()) {
							if (fullVertices.size() == 256) {
								Logger::error("Failed to load shape, Max of 256 vertices " + path);
								fclose(shapeFile);
								return;
							}

							fullVertices.push_back(point);
							fileIndices.push_back(fullVertices.size() - 1);
						}
						else {
							fileIndices.push_back(it - fullVertices.begin());
						}
					}
				}
				else {

					char c;
					do {
						c = fgetc(shapeFile);
					} while (c != '\n');
				}
			}
		}

		fclose(shapeFile);

		indexCount = fileIndices.size();
		vertexCount = fullVertices.size();

		if (indexCount == 0 || vertexCount == 0) {
			indexCount = 0;
			vertexCount = 0;
			Logger::error(" Failed to load block shape " + name + " mesh was empty");
			return;
		}

		indices = (uint8_t*)malloc(indexCount * sizeof(uint8_t));
		if (indices == nullptr) {
			indexCount = 0;
			vertexCount = 0;
			Logger::error("Failed to allocate index buffer for block shape " + name);
			return;
		}

		memcpy(indices, fileIndices.data(), indexCount * sizeof(uint8_t));
		vertices = (float*)malloc(vertexCount * 5 * sizeof(float));
		if (vertices == nullptr) {
			indexCount = 0;
			vertexCount = 0;
			Logger::error("Failed to allocate vertex buffer for block shape " + name);
			free(indices);
			indices = nullptr;
			return;
		}

		memcpy(vertices, fullVertices.data(), vertexCount * 5 * sizeof(float));
		Logger::debug("Loaded block shape " + name + " index count: " + std::to_string(indexCount) + " vertex count: " + std::to_string(vertexCount) + " path: " + path);
	}

	Shape::~Shape()
	{
		free(indices);
		free(vertices);
	}

	std::string Shape::getFileName(const std::string& path)
	{
		std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
		return base_filename.substr(0, base_filename.find_last_of('.'));
	}
}
