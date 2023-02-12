#include "RenderLine.h"

#include "PxeBuffer.h"
#include "PxeShader.h"
#include "PxeEngine.h"
#include "PxeFSHelpers.h"
#include "GL/glew.h"

namespace pixelexplorer {
	namespace tools {
		pxengine::PxeRenderMaterial* RenderLine::s_material = nullptr;
		pxengine::PxeVertexArray* RenderLine::s_vertexArray = nullptr;
		pxengine::PxeVertexBuffer* RenderLine::s_vertexBuffer = nullptr;
		pxengine::PxeIndexBuffer* RenderLine::s_indexBuffer = nullptr;
		pxengine::PxeBuffer* RenderLine::s_vertexData = nullptr;
		std::mutex RenderLine::s_renderDataMutex;

		RenderLine* RenderLine::createLine() {
			using namespace pxengine;
			static constexpr uint8_t indices[] = { 0, 1, 2, 0, 2, 1, 0, 2, 3, 0, 3, 2, 4, 5, 6, 4, 6, 5, 4, 6, 7, 4, 7, 6 };

			std::lock_guard lock(s_renderDataMutex);
			if (!s_material) {
				PxeShader* shader = pxeGetEngine().loadShader(getAssetPath("shaders") / "basic.pxeshader");
				if (!shader) return nullptr;
				s_material = new(std::nothrow) PxeRenderMaterial(*shader);
				shader->drop();

				if (!s_material) {
					return nullptr;
				}

				PxeBuffer* indexBuf = new(std::nothrow) PxeBuffer(sizeof(indices));
				if (!indexBuf) {
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				if (!indexBuf->getAllocated()) {
					indexBuf->drop();
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				memcpy(indexBuf->getBuffer(), indices, sizeof(indices));
				s_indexBuffer = new(std::nothrow) PxeIndexBuffer(PxeIndexType::UNSIGNED_8BIT, indexBuf);
				indexBuf->drop();
				if (!s_indexBuffer) {
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				s_vertexData = new(std::nothrow) PxeBuffer(sizeof(float) * 8 * 3);
				if (!s_vertexData) {
					s_indexBuffer->drop();
					s_indexBuffer = nullptr;
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				if (!s_vertexData->getAllocated()) {
					s_vertexData->drop();
					s_vertexData = nullptr;
					s_indexBuffer->drop();
					s_indexBuffer = nullptr;
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				PxeVertexBufferFormat fmt(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
				s_vertexBuffer = new(std::nothrow) PxeVertexBuffer(fmt, s_vertexData);
				if (!s_vertexBuffer) {
					s_vertexData->drop();
					s_vertexData = nullptr;
					s_indexBuffer->drop();
					s_indexBuffer = nullptr;
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				s_vertexArray = new(std::nothrow) PxeVertexArray();
				if (!s_vertexArray) {
					s_vertexBuffer->drop();
					s_vertexBuffer = nullptr;
					s_vertexData->drop();
					s_vertexData = nullptr;
					s_indexBuffer->drop();
					s_indexBuffer = nullptr;
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				s_vertexArray->setIndexBuffer(s_indexBuffer);
				s_vertexArray->addVertexBuffer(*s_vertexBuffer, 0, 0);
				s_material->setProperty4f("u_Color", glm::vec4(1, 0, 1, 1));
			}

			RenderLine* line = new(std::nothrow) RenderLine(*s_material);
			if (!line) {
				if (s_material->getRefCount() == 1) {
					s_vertexArray->drop();
					s_vertexArray = nullptr;
					s_vertexBuffer->drop();
					s_vertexBuffer = nullptr;
					s_vertexData->drop();
					s_vertexData = nullptr;
					s_indexBuffer->drop();
					s_indexBuffer = nullptr;
					s_material->drop();
					s_material = nullptr;
				}

				return nullptr;
			}

			return line;
		}

		RenderLine::RenderLine(pxengine::PxeRenderMaterial& material) : pxengine::PxeRenderObject(material) {
			Disabled = true;
			Width = 0.1f;
			Color = glm::vec4(1, 0, 1, 1);
		};

		RenderLine::~RenderLine() {
			std::lock_guard lock(s_renderDataMutex);
			if (s_material->getRefCount() == 2) {
				s_vertexArray->drop();
				s_vertexArray = nullptr;
				s_vertexBuffer->drop();
				s_vertexBuffer = nullptr;
				s_vertexData->drop();
				s_vertexData = nullptr;
				s_indexBuffer->drop();
				s_indexBuffer = nullptr;
				s_material->drop();
				s_material = nullptr;
			}
		}

		void RenderLine::onGeometry() {
			if (Disabled) return;
			s_material->setProperty4f("u_Color", Color);
			s_material->applyMaterial();
			glm::vec3* points = static_cast<glm::vec3*>(s_vertexData->getBuffer());
			float halfWidth = Width / 2.0f;
			points[0] = PointA + glm::vec3(0, -halfWidth, 0);
			points[1] = PointB + glm::vec3(0, -halfWidth, 0);
			points[2] = PointB + glm::vec3(0, halfWidth, 0);
			points[3] = PointA + glm::vec3(0, halfWidth, 0);

			points[4] = PointA + glm::vec3(-halfWidth, 0, 0);
			points[5] = PointB + glm::vec3(-halfWidth, 0, 0);
			points[6] = PointB + glm::vec3(halfWidth, 0, 0);
			points[7] = PointA + glm::vec3(halfWidth, 0, 0);

			s_vertexBuffer->bufferData(*s_vertexData);
			s_vertexArray->bind();
			glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, nullptr);
		}
	}
}