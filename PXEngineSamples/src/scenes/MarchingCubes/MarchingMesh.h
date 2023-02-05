#ifndef PXENGINESAMPLES_MARCHING_CUBES_MESH_H_
#define PXENGINESAMPLES_MARCHING_CUBES_MESH_H_
#include <new>
#include <malloc.h>
#include <cassert>

#include "PxeTypes.h"
#include "MarchingCubesLookupTable.h"
#include "PxeRenderObject.h"
#include "PxeRenderMaterial.h"
#include "PxeVertexArray.h"
#include "PxeVertexBuffer.h"
#include "PxeIndexBuffer.h"
#include "GL/glew.h"
#include "SDL.h"

namespace marchingcubes {
	class MarchingMesh : public pxengine::PxeRenderObject
	{
	public:
		MarchingMesh(pxengine::PxeRenderMaterial& material, uint16_t gridSize) : PxeRenderObject(material) {
			_gridPoints = static_cast<uint8_t*>(calloc(gridSize * gridSize * gridSize, sizeof(uint8_t)));
			assert(_gridPoints);
			_gridSize = gridSize;
			_indexBuffer = new pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_32BIT);
			pxengine::PxeVertexBufferFormat vertexFormat(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
			_vertexBuffer = new pxengine::PxeVertexBuffer(vertexFormat);
			_vertexArray = new pxengine::PxeVertexArray();
			_vertexArray->addVertexBuffer(*_vertexBuffer, 0, 0);
			positionMatrix = glm::translate(positionMatrix, glm::vec3(-gridSize / 2, -gridSize / 2, 20));
			positionMatrix = glm::rotate(positionMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
			recalculateMesh();
		}

		virtual ~MarchingMesh() {
			_vertexArray->drop();
			_vertexBuffer->drop();
			_indexBuffer->drop();
			free(_gridPoints);
		}

		uint8_t* getGridPoints() const { return _gridPoints; }

		uint16_t getGridSize() const { return _gridSize; }

		uint8_t& getPoint(const glm::u16vec3& pos) const {
			return _gridPoints[(pos.z * _gridSize * _gridSize) + (pos.y * _gridSize) + pos.x];
		}

		const glm::u16vec3& getPointOffset(uint8_t index) const {
			const static glm::u16vec3 offsets[8] = {
				glm::u16vec3(0,0,0),
				glm::u16vec3(1,0,0),
				glm::u16vec3(1,0,1),
				glm::u16vec3(0,0,1),
				glm::u16vec3(0,1,0),
				glm::u16vec3(1,1,0),
				glm::u16vec3(1,1,1),
				glm::u16vec3(0,1,1)
			};

			return offsets[index];
		}

		const glm::vec3& getVertexPoint(uint8_t index) const {
			const static glm::vec3 points[12] = {
				glm::vec3(0.5f, 0.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.5f),
				glm::vec3(0.5f, 0.0f, 1.0f),
				glm::vec3(0.0f, 0.0f, 0.5f),
				glm::vec3(0.5f, 1.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 0.5f),
				glm::vec3(0.5f, 1.0f, 1.0f),
				glm::vec3(0.0f, 1.0f, 0.5f),
				glm::vec3(0.0f, 0.5f, 0.0f),
				glm::vec3(1.0f, 0.5f, 0.0f),
				glm::vec3(1.0f, 0.5f, 1.0f),
				glm::vec3(0.0f, 0.5f, 1.0f)
			};

			return points[index];
		}

		void recalculateMesh() {
			uint32_t vertexFloatCount = 0;
			uint32_t indexValueCount = 0;
			for (uint16_t x = 0; x < _gridSize - 1; ++x) {
				for (uint16_t y = 0; y < _gridSize - 1; ++y) {
					for (uint16_t z = 0; z < _gridSize - 1; ++z) {
						uint8_t triIndex = 0;
						for (uint8_t i = 0; i < 8; ++i) {
							if (getPoint(glm::u16vec3(x, y, z) + getPointOffset(i)))
								triIndex |= (1 << i);
						}

						const int8_t* trianglePoints = TriangleTable[triIndex];
						for (uint8_t i = 0; i < 16; ++i) {
							if (trianglePoints[i] == -1) break;
							indexValueCount += 1;
							vertexFloatCount += 3;
						}
					}
				}
			}

			pxengine::PxeBuffer* indexData = new pxengine::PxeBuffer(indexValueCount * _indexBuffer->getIndexSize());
			pxengine::PxeBuffer* vertexData = new pxengine::PxeBuffer(vertexFloatCount * sizeof(float));
			uint32_t* indexBuffer = static_cast<uint32_t*>(indexData->getBuffer());
			float* vertexBuffer = static_cast<float*>(vertexData->getBuffer());
			if (indexValueCount && vertexFloatCount) {
				assert(indexBuffer);
				assert(vertexBuffer);
				vertexFloatCount = 0;
				indexValueCount = 0;

				for (uint16_t x = 0; x < _gridSize - 1; ++x) {
					for (uint16_t y = 0; y < _gridSize - 1; ++y) {
						for (uint16_t z = 0; z < _gridSize - 1; ++z) {
							uint8_t triIndex = 0;
							for (uint8_t i = 0; i < 8; ++i) {
								if (getPoint(glm::u16vec3(x, y, z) + getPointOffset(i)))
									triIndex |= (1 << i);
							}

							const int8_t* trianglePoints = TriangleTable[triIndex];
							for (uint8_t i = 0; i < 16; ++i) {
								if (trianglePoints[i] == -1) break;
								const glm::vec3& point = glm::vec3(x, y, z) + getVertexPoint(trianglePoints[i]);
								indexBuffer[indexValueCount++] = vertexFloatCount / 3;
								vertexBuffer[vertexFloatCount++] = point.x;
								vertexBuffer[vertexFloatCount++] = point.y;
								vertexBuffer[vertexFloatCount++] = point.z;
							}

						}
					}
				}
			}

			_indexBuffer->bufferData(*indexData);
			_vertexBuffer->bufferData(*vertexData);
			vertexData->drop();
			indexData->drop();
		}

		void onGeometry() override {
			_vertexArray->bind();
			_vertexBuffer->bind();
			_indexBuffer->bind();
			glDrawElements(GL_TRIANGLES, _indexBuffer->getIndexCount(), (uint32_t)_indexBuffer->getIndexType(), nullptr);
			_indexBuffer->unbind();
			_vertexArray->unbind();
		}

	private:
		pxengine::PxeVertexArray* _vertexArray;
		pxengine::PxeVertexBuffer* _vertexBuffer;
		pxengine::PxeIndexBuffer* _indexBuffer;
		uint8_t* _gridPoints;
		uint16_t _gridSize;
	};
}

#endif // !PXENGINESAMPLES_MARCHING_CUBES_MESH_H_
