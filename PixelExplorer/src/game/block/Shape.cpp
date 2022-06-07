#include "Shape.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unordered_map>

#include "Logger.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace pixelexplorer::game::block {
	Shape::Shape(const std::string& path) : name(getFileName(path))
	{
		memset(&indices, 0, sizeof(indices));
		memset(&vertices, 0, sizeof(vertices));
		memset(&vertexCount, 0, sizeof(vertexCount));
		memset(&indexCount, 0, sizeof(indexCount));
		loadShapeFile(path);
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
		memset(&indices, 0, sizeof(indices));
		memset(&vertices, 0, sizeof(vertices));
		memset(&vertexCount, 0, sizeof(vertexCount));
		memset(&indexCount, 0, sizeof(indexCount));
	}

	void Shape::loadShapeFile(const std::string& path)
	{
		enum class ShapeDirection { NONE = -1, FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM };

		FILE* shape = nullptr;
		errno_t fileError = fopen_s(&shape, path.c_str(), "r");
		if (fileError != 0 || shape == nullptr) {
			Logger::error("Failed to open shape file " + path + ", error: " + std::to_string(fileError));
			return;
		}

		freeShape();
		ShapeDirection currentDirection = ShapeDirection::NONE;
		char let;
		while ((let = fgetc(shape)) && !feof(shape))
		{
			switch (let) {
			case 'd': {
				char dirStr[7] = { 0,0,0,0,0,0,0 };
				for (int8_t i = 0; i < 6; ++i) {
					dirStr[i] = fgetc(shape);
					if (i == 0 && dirStr[0] == ' ') {
						--i;
						continue;
					}

					if (dirStr[i] == '\n') {
						dirStr[i] = 0;
						break;
					}
				}

				if (strcmp(dirStr, "FRONT") == 0) {
					currentDirection = ShapeDirection::FRONT;
				}
				else if (strcmp(dirStr, "BACK") == 0) {
					currentDirection = ShapeDirection::BACK;
				}
				else if (strcmp(dirStr, "LEFT") == 0) {
					currentDirection = ShapeDirection::LEFT;
				}
				else if (strcmp(dirStr, "RIGHT") == 0) {
					currentDirection = ShapeDirection::RIGHT;
				}
				else if (strcmp(dirStr, "TOP") == 0) {
					currentDirection = ShapeDirection::TOP;
				}
				else if (strcmp(dirStr, "BOTTOM") == 0) {
					currentDirection = ShapeDirection::BOTTOM;
				}
				else {
					Logger::error("Failed to parse shape file " + path + " invalid direction: " + dirStr);
					fclose(shape);
					return;
				}
			} break;

			case 'v':
				if (currentDirection == ShapeDirection::NONE) {
					Logger::error("Failed to parse shape file " + path + " found vertex before direction");
					fclose(shape);
					return;
				}

				++vertexCount[(int32_t)currentDirection];
				readTillLineEnd(shape);
				break;

			case 'f':
				if (currentDirection == ShapeDirection::NONE) {
					Logger::error("Failed to parse shape file " + path + " found face before direction");
					fclose(shape);
					return;
				}

				indexCount[(int32_t)currentDirection] += 3;
				readTillLineEnd(shape);
				break;

			default:
				readTillLineEnd(shape);
				break;
			}
		}

		rewind(shape);
		indices[0] = (uint8_t*)malloc((indexCount[0] + indexCount[1] + indexCount[2] + indexCount[3] + indexCount[4] + indexCount[5]) * sizeof(uint8_t));
		vertices[0] = (float*)malloc((vertexCount[0] + vertexCount[1] + vertexCount[2] + vertexCount[3] + vertexCount[4] + vertexCount[5]) * 5 * sizeof(float));
		for (uint8_t i = 1; i < 6; ++i) {
			indices[i] = indices[i - 1] + indexCount[i - 1];
			vertices[i] = vertices[i - 1] + (vertexCount[i - 1] * 5);
		}

		if (vertices[0] == nullptr || indices[0] == nullptr) {
			Logger::error("Failed to allocate shape buffers for shape " + name);
			freeShape();
			fclose(shape);
			return;
		}

		memset(&vertexCount, 0, sizeof(vertexCount));
		memset(&indexCount, 0, sizeof(indexCount));

		while ((let = fgetc(shape)) && !feof(shape))
		{
			switch (let) {
			case 'd': {
				char dirStr[7] = { 0,0,0,0,0,0,0 };
				for (uint8_t i = 0; i < 6; ++i) {
					dirStr[i] = fgetc(shape);
					if (i == 0 && dirStr[0] == ' ') {
						--i;
						continue;
					}

					if (dirStr[i] == '\n') {
						dirStr[i] = 0;
						break;
					}
				}

				if (strcmp(dirStr, "FRONT") == 0) {
					currentDirection = ShapeDirection::FRONT;
				}
				else if (strcmp(dirStr, "BACK") == 0) {
					currentDirection = ShapeDirection::BACK;
				}
				else if (strcmp(dirStr, "LEFT") == 0) {
					currentDirection = ShapeDirection::LEFT;
				}
				else if (strcmp(dirStr, "RIGHT") == 0) {
					currentDirection = ShapeDirection::RIGHT;
				}
				else if (strcmp(dirStr, "TOP") == 0) {
					currentDirection = ShapeDirection::TOP;
				}
				else if (strcmp(dirStr, "BOTTOM") == 0) {
					currentDirection = ShapeDirection::BOTTOM;
				}
				else {
					Logger::error("Failed to parse shape file " + path + " invalid direction: " + dirStr);
					freeShape();
					fclose(shape);
					return;
				}
			} break;

			case 'v': {
				if (currentDirection == ShapeDirection::NONE) {
					Logger::error("Failed to parse shape file " + path + " found vertex before direction");
					fclose(shape);
					freeShape();
					return;
				}

				float* vp = vertices[(int32_t)currentDirection] + (vertexCount[(int32_t)currentDirection] * 5);
				if (fscanf_s(shape, "%f %f %f %f %f\n", vp, vp + 1, vp + 2, vp + 3, vp + 4) != 5) {
					fclose(shape);
					freeShape();
					Logger::error("Failed to parse shape file " + path + " found invalid vertex");
					return;
				}

				++vertexCount[(int32_t)currentDirection];
			} break;

			case 'f': {
				if (currentDirection == ShapeDirection::NONE) {
					Logger::error("Failed to parse shape file " + path + " found face before direction");
					fclose(shape);
					freeShape();
					return;
				}

				uint8_t* ip = indices[(int32_t)currentDirection] + indexCount[(int32_t)currentDirection];
				int32_t values[3];
				if (fscanf_s(shape, "%i/%i/%i\n", values, values + 1, values + 2) != 3) {
					fclose(shape);
					freeShape();
					Logger::error("Failed to parse shape file " + path + " found invalid face");
					return;
				}

				ip[0] = values[0];
				ip[1] = values[1];
				ip[2] = values[2];
				indexCount[(int32_t)currentDirection] += 3;
			} break;

			default:
				readTillLineEnd(shape);
				break;
			}
		}

		fclose(shape);
		Logger::debug("Loaded shape " + name + " from " + path);
	}

	void Shape::readTillLineEnd(FILE* file)
	{
		char c;
		do
		{
			c = fgetc(file);
		} while (c != '\n' && !feof(file));
	}
}
