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
		memset(indices, 0, sizeof(indices));
		memset(vertices, 0, sizeof(vertices));
		memset(vertexCount, 0, sizeof(vertexCount));
		memset(indexCount, 0, sizeof(indexCount));

		FILE* shapeFile;
		if (fopen_s(&shapeFile, path.c_str(), "r") != 0 || shapeFile == nullptr) {
			Logger::error("Failed to open shape file " + path);
			return;
		}

		enum class MeshDirection { NONE = -1, FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };
		MeshDirection direction = MeshDirection::NONE;
		uint32_t totalVertexCount = 0;
		uint32_t totalIndexCount = 0;
		char lineStart[3] = { 0,0,0 };

		while (fread(lineStart, 1, 2, shapeFile) == 2) {
			if (strcmp(lineStart, "d ") == 0) {
				char dir[7] = { 0,0,0,0,0,0,0 };
				for (uint8_t i = 0; i < 6; ++i) {
					dir[i] = fgetc(shapeFile);
					if (dir[i] == '\n') {
						dir[i] = 0;
						break;
					}
				}

				if (strcmp(dir, "FRONT") == 0) {
					direction = MeshDirection::FRONT;
				}
				else if (strcmp(dir, "BACK") == 0) {
					direction = MeshDirection::BACK;
				}
				else if (strcmp(dir, "LEFT") == 0) {
					direction = MeshDirection::LEFT;
				}
				else if (strcmp(dir, "RIGHT") == 0) {
					direction = MeshDirection::RIGHT;
				}
				else if (strcmp(dir, "TOP") == 0) {
					direction = MeshDirection::TOP;
				}
				else if (strcmp(dir, "BOTTOM") == 0) {
					direction = MeshDirection::BOTTOM;
				}
				else {
					fclose(shapeFile);
					Logger::error("Failed to parse shape file, found invalid direction " + path);
					return;
				}
			}
			else if (strcmp(lineStart, "vc") == 0) {
				if (direction == MeshDirection::NONE) {
					fclose(shapeFile);
					Logger::error("Failed to parse shape file, found invalid direction " + path);
					return;
				}

				int32_t vc;
				fscanf_s(shapeFile, "%i\n", &vc);
				totalVertexCount += vc * 5;
				vertexCount[(int32_t)direction] = vc;
			}
			else if (strcmp(lineStart, "fc") == 0) {
				if (direction == MeshDirection::NONE) {
					fclose(shapeFile);
					Logger::error("Failed to parse shape file, found invalid direction " + path);
					return;
				}

				int32_t fc;
				fscanf_s(shapeFile, "%i\n", &fc);
				totalIndexCount += fc * 3;
				indexCount[(int32_t)direction] += fc * 3;
			}
			else {
				char c;
				do {
					c = fgetc(shapeFile);
				} while (c != '\n' && !feof(shapeFile));
			}
		}

		indices[0] = (uint8_t*)malloc(totalIndexCount * sizeof(uint8_t));
		vertices[0] = (float*)malloc(totalVertexCount * sizeof(float));
		for (uint8_t i = 1; i < 6; ++i) {
			indices[i] = indices[i - 1] + indexCount[i - 1];
			vertices[i] = vertices[i - 1] + vertexCount[i - 1] * 5;
		}

		uint32_t loadedVertexCount = 0;
		uint32_t loadedIndexCount = 0;
		rewind(shapeFile);
		while (fread(lineStart, 1, 2, shapeFile) == 2)
		{
			if (strcmp(lineStart, "d ") == 0) {
				char dir[7] = { 0,0,0,0,0,0,0 };
				for (uint8_t i = 0; i < 6; ++i) {
					dir[i] = fgetc(shapeFile);
					if (dir[i] == '\n') {
						dir[i] = 0;
						break;
					}
				}

				if (strcmp(dir, "FRONT") == 0) {
					direction = MeshDirection::FRONT;
				}
				else if (strcmp(dir, "BACK") == 0) {
					direction = MeshDirection::BACK;
				}
				else if (strcmp(dir, "LEFT") == 0) {
					direction = MeshDirection::LEFT;
				}
				else if (strcmp(dir, "RIGHT") == 0) {
					direction = MeshDirection::RIGHT;
				}
				else if (strcmp(dir, "TOP") == 0) {
					direction = MeshDirection::TOP;
				}
				else if (strcmp(dir, "BOTTOM") == 0) {
					direction = MeshDirection::BOTTOM;
				}
				else {
					fclose(shapeFile);
					freeShape();
					Logger::error("Failed to parse shape file, found invalid direction " + path);
					return;
				}
			}
			else if (strcmp(lineStart, "v ") == 0) {
				if (direction == MeshDirection::NONE) {
					fclose(shapeFile);
					Logger::error("Failed to parse shape file, found invalid direction " + path);
					return;
				}

				if (loadedVertexCount + 5 > totalVertexCount || fscanf_s(shapeFile, "%f %f %f %f %f\n", vertices[0] + loadedVertexCount, vertices[0] + loadedVertexCount + 1, vertices[0] + loadedVertexCount + 2, vertices[0] + loadedVertexCount + 3, vertices[0] + loadedVertexCount + 4) != 5) {
					fclose(shapeFile);
					freeShape();
					Logger::error("Failed to parse shape file, found invalid vertex count " + path);
					return;
				}

				loadedVertexCount += 5;
			}
			else if (strcmp(lineStart, "f ") == 0) {
				if (direction == MeshDirection::NONE) {
					fclose(shapeFile);
					Logger::error("Failed to parse shape file, found invalid direction " + path);
					return;
				}

				if (loadedIndexCount + 3 > totalIndexCount || fscanf_s(shapeFile, "%i/%i/%i\n", indices[0] + loadedIndexCount, indices[0] + loadedIndexCount + 1, indices[0] + loadedIndexCount + 2) != 3) {
					fclose(shapeFile);
					freeShape();
					Logger::error("Failed to parse shape file, found invalid index count " + path);
					return;
				}

				loadedIndexCount += 3;
			}
			else {

				char c;
				do {
					c = fgetc(shapeFile);
				} while (c != '\n' && !feof(shapeFile));
			}
		}

		fclose(shapeFile);
		if (loadedIndexCount != totalIndexCount) {
			freeShape();
			Logger::error("Failed to parse shape file, found invalid index count " + path);
			return;
		}

		if (loadedVertexCount != totalVertexCount) {
			freeShape();
			Logger::error("Failed to parse shape file, found invalid vertex count " + path);
			return;
		}

		Logger::debug("Loaded block shape " + name + " from " + path + " loaded: " + std::to_string(totalIndexCount) + " indices, " + std::to_string(totalVertexCount) + " vertices");

		// OBJ file loading
		//FILE* shapeFile;
		//if (fopen_s(&shapeFile, path.c_str(), "r") != 0 || shapeFile == nullptr) {
		//	Logger::error("Failed to open shape file " + path);
		//	return;
		//}

		//std::vector<uint8_t> fileIndices;
		//std::vector<Vec5> fullVertices;

		//{
		//	std::vector<glm::vec3> fileVertices;
		//	std::vector<glm::vec2> fileUVs;
		//	char lineStart[3];
		//	lineStart[2] = 0;

		//	while (fread(lineStart, 1, 2, shapeFile) == 2)
		//	{
		//		if (strcmp(lineStart, "v ") == 0) {
		//			glm::vec3 v;
		//			fscanf_s(shapeFile, "%f %f %f\n", &v.x, &v.y, &v.z);
		//			fileVertices.push_back(v);
		//		}
		//		else if (strcmp(lineStart, "vt") == 0) {
		//			glm::vec2 v;
		//			fscanf_s(shapeFile, "%f %f\n", &v.x, &v.y);
		//			fileUVs.push_back(v);
		//		}
		//		else if (strcmp(lineStart, "f ") == 0) {
		//			int32_t vertexIndex[3], uvIndex[3], normalIndex[3];
		//			if (fscanf_s(shapeFile, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]) != 9) {
		//				Logger::error("Failed to parse shape file " + path);
		//				return;
		//			}

		//			for (uint8_t i = 0; i < 3; ++i) {
		//				//if (indexCount == 0xff) {
		//				//	Logger::error("Failed to load shape, Max of 255 indices " + path);
		//				//	fclose(shapeFile);
		//				//	return;
		//				//}

		//				Vec5 point;
		//				point.x = fileVertices[vertexIndex[i] - 1].x;
		//				point.y = fileVertices[vertexIndex[i] - 1].y;
		//				point.z = fileVertices[vertexIndex[i] - 1].z;
		//				point.u = fileUVs[uvIndex[i] - 1].x;
		//				point.v = fileUVs[uvIndex[i] - 1].y;

		//				auto it = std::find(fullVertices.begin(), fullVertices.end(), point);
		//				if (it == fullVertices.end()) {
		//					if (fullVertices.size() == 256) {
		//						Logger::error("Failed to load shape, Max of 256 vertices " + path);
		//						fclose(shapeFile);
		//						return;
		//					}

		//					fullVertices.push_back(point);
		//					fileIndices.push_back(fullVertices.size() - 1);
		//				}
		//				else {
		//					fileIndices.push_back(it - fullVertices.begin());
		//				}
		//			}
		//		}
		//		else {

		//			char c;
		//			do {
		//				c = fgetc(shapeFile);
		//			} while (c != '\n');
		//		}
		//	}
		//}

		//fclose(shapeFile);

		//indexCount = fileIndices.size();
		//vertexCount = fullVertices.size();

		//if (indexCount == 0 || vertexCount == 0) {
		//	indexCount = 0;
		//	vertexCount = 0;
		//	Logger::error(" Failed to load block shape " + name + " mesh was empty");
		//	return;
		//}

		//indices = (uint8_t*)malloc(indexCount * sizeof(uint8_t));
		//if (indices == nullptr) {
		//	indexCount = 0;
		//	vertexCount = 0;
		//	Logger::error("Failed to allocate index buffer for block shape " + name);
		//	return;
		//}

		//memcpy(indices, fileIndices.data(), indexCount * sizeof(uint8_t));
		//vertices = (float*)malloc(vertexCount * 5 * sizeof(float));
		//if (vertices == nullptr) {
		//	indexCount = 0;
		//	vertexCount = 0;
		//	Logger::error("Failed to allocate vertex buffer for block shape " + name);
		//	free(indices);
		//	indices = nullptr;
		//	return;
		//}

		//memcpy(vertices, fullVertices.data(), vertexCount * 5 * sizeof(float));
		//Logger::debug("Loaded block shape " + name + " index count: " + std::to_string(indexCount) + " vertex count: " + std::to_string(vertexCount) + " path: " + path);
	}

	Shape::~Shape()
	{
		freeShape();
	}

	std::string Shape::getFileName(const std::string& path)
	{
		std::string base_filename = path.substr(path.find_last_of("/\\") + 1);
		return base_filename.substr(0, base_filename.find_last_of('.'));
	}

	void Shape::freeShape()
	{
		free(indices[0]);
		free(vertices[0]);
		memset(indices, 0, sizeof(indices));
		memset(vertices, 0, sizeof(vertices));
		memset(vertexCount, 0, sizeof(vertexCount));
		memset(indexCount, 0, sizeof(indexCount));
	}
}
