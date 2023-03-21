#include "RenderPoint.h"

#include "PxeBuffer.h"
#include "PxeShader.h"
#include "PxeEngine.h"
#include "PxeFSHelpers.h"
#include "GL/glew.h"

namespace pixelexplorer {
	namespace tools {
		pxengine::PxeUnlitRenderMaterial* RenderPoint::s_material = nullptr;
		pxengine::PxeVertexArray* RenderPoint::s_vertexArray = nullptr;
		std::mutex RenderPoint::s_renderDataMutex;

		RenderPoint* RenderPoint::createPoint() {
			using namespace pxengine;
			static constexpr uint8_t indices[36] = { 0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 4, 7, 0, 7, 3, 1, 6, 5, 1, 2, 6, 3, 7, 6, 3, 6, 2, 0, 5, 4, 0, 1, 5 };
			static constexpr float vertices[] = { -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,-0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f,-0.5f,  0.5f,  0.5f };
			
			std::lock_guard lock(s_renderDataMutex);
			if (!s_material) {
				s_material = PxeUnlitRenderMaterial::createMaterial();
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
				PxeIndexBuffer* indexBuffer = new(std::nothrow) PxeIndexBuffer(PxeIndexType::UNSIGNED_8BIT, indexBuf);
				indexBuf->drop();
				if (!indexBuffer) {
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				PxeBuffer* vertexBuf = new(std::nothrow) PxeBuffer(sizeof(vertices));
				if (!vertexBuf) {
					indexBuffer->drop();
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				if (!vertexBuf->getAllocated()) {
					vertexBuf->drop();
					indexBuffer->drop();
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				memcpy(vertexBuf->getBuffer(), vertices, sizeof(vertices));
				PxeVertexBufferFormat fmt(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
				PxeVertexBuffer* vertexBuffer = new(std::nothrow) PxeVertexBuffer(fmt, vertexBuf);
				vertexBuf->drop();

				if (!vertexBuffer) {
					indexBuffer->drop();
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				s_vertexArray = new(std::nothrow) PxeVertexArray();
				if (!s_vertexArray) {
					vertexBuffer->drop();
					indexBuffer->drop();
					s_material->drop();
					s_material = nullptr;
					return nullptr;
				}

				s_vertexArray->setIndexBuffer(indexBuffer);
				indexBuffer->drop();
				s_vertexArray->addVertexBuffer(*vertexBuffer, 0, 0);
				vertexBuffer->drop();
			}

			RenderPoint* point = new(std::nothrow) RenderPoint(*s_material);
			if (!point) {
				if (s_material->getRefCount() == 1) {
					s_vertexArray->drop();
					s_vertexArray = nullptr;
					s_material->drop();
					s_material = nullptr;
				}

				return nullptr;
			}

			return point;
		}

		RenderPoint::RenderPoint(pxengine::PxeRenderMaterialInterface& material) : pxengine::PxeRenderObject(material) {
			Disabled = true;
			Width = 1;
			Color = glm::vec4(1, 0, 1, 1);
		};

		RenderPoint::~RenderPoint() {
			std::lock_guard lock(s_renderDataMutex);
			if (s_material->getRefCount() == 2) {
				s_vertexArray->drop();
				s_vertexArray = nullptr;
				s_material->drop();
				s_material = nullptr;
			}
		}

		void RenderPoint::onRender() {
			if (Disabled) return;
			s_material->getShader().setUniform4f("u_Color", Color);
			s_material->applyMaterial();
			positionMatrix = glm::scale(glm::translate(glm::mat4(1), Point), glm::vec3(Width));
			s_vertexArray->bind();
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);
		}
	}
}