#include <stdint.h>

#include "RefCount.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_RENDERMESH_H_
#define  PIXELEXPLORE_RENDERING_RENDERMESH_H_
namespace pixelexplore::rendering {
	class RenderMesh : public RefCount
	{
		friend class RenderWindow;
	public:
		RenderMesh();
		virtual ~RenderMesh();
		virtual Shader* getShader() = 0;
		virtual inline const glm::mat4 getPositionMatrix() const { return positionMatrix; }
		inline void setBuffersDirty() { _buffersDirty = true; }
		inline bool getBuffersDirty() const { return _buffersDirty; }

	protected:
		glm::mat4 positionMatrix;
		uint32_t indexCount;
		uint32_t indexType;

		virtual void updateGlObjects();
		virtual void deleteGlObjects();
		virtual void createGlObjects();
		virtual inline bool shouldDrawMesh() { return true; }
		virtual void drawMesh();
		inline uint32_t getVertextArrayGlId() const { return _vertexArrayGlId; }
		inline uint32_t getVertextBufferGlId() const { return _vertexBufferGlId; }
		inline uint32_t getIndexBufferGlId() const { return _indexBufferGlId; }

	private:
		uint32_t _vertexArrayGlId;
		uint32_t _vertexBufferGlId;
		uint32_t _indexBufferGlId;
		bool _hasGlObjects;
		bool _buffersDirty;
	};
}
#endif // !PIXELEXPLORE_RENDERING_RENDERMESH_H_
