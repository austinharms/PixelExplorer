#ifndef PXENGINESAMPLES_PHYSICS_RENDERABLE_H_
#define PXENGINESAMPLES_PHYSICS_RENDERABLE_H_
#include "PxeEngineAPI.h"
#include "PxPhysicsAPI.h"
#include "PxShape.h"
#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"

namespace cubestack {
	class PhysicsCube : public pxengine::PxeDynamicPhysicsRenderObject
	{
	public:
		static PhysicsCube* createPhysicsCube(const glm::vec3& pos, bool dynamic, pxengine::PxeRenderMaterial& material, pxengine::PxeIndexBuffer* indexBuffer = nullptr, pxengine::PxeVertexArray* vertextArray = nullptr, physx::PxShape* shape = nullptr, const glm::vec3& velocity = glm::vec3(0)) {
			physx::PxPhysics& physics = pxengine::pxeGetEngine().getPhysicsBase();
			physx::PxRigidActor* actor;
			physx::PxTransform pxTransform(physx::PxVec3(pos.x, pos.y, pos.z));
			if (dynamic) {
				actor = physics.createRigidDynamic(pxTransform);
				static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z));
			}
			else {
				actor = physics.createRigidStatic(pxTransform);
			}

			if (!actor) return nullptr;
			PhysicsCube* renderable = new(std::nothrow) PhysicsCube(*actor, material, indexBuffer, vertextArray, shape);
			if (!renderable) {
				actor->release();
				return nullptr;
			}

			renderable->positionMatrix = glm::translate(renderable->positionMatrix, pos);
			return renderable;
		}

		virtual ~PhysicsCube() {
			_indexBuffer->drop();
			_vertexArray->drop();
			_pxShape->release();
		}

		void onRender() override {
			_vertexArray->bind();
			_indexBuffer->bind();
			glDrawElements(GL_TRIANGLES, 36, (uint32_t)_indexBuffer->getIndexType(), nullptr);
			_indexBuffer->unbind();
			_vertexArray->unbind();
		}

		pxengine::PxeIndexBuffer* getIndexBuffer() const { return _indexBuffer; }

		pxengine::PxeVertexArray* getVertexArray() const { return _vertexArray; }

		physx::PxShape* getShape() const { return _pxShape; }

	private:
		PhysicsCube(physx::PxRigidActor& actor, pxengine::PxeRenderMaterial& material, pxengine::PxeIndexBuffer* indexBuffer, pxengine::PxeVertexArray* vertextArray, physx::PxShape* shape) : pxengine::PxeDynamicPhysicsRenderObject(material, &actor) {
			uint16_t indices[36] = {
				0, 2, 1, 0, 3, 2,  // Front
				4, 5, 6, 4, 6, 7,  // Back
				0, 4, 7, 0, 7, 3,  // Left
				1, 6, 5, 1, 2, 6,  // Right
				3, 7, 6, 3, 6, 2,  // Top
				0, 5, 4, 0, 1, 5,  // Bottom
			};

			float vertices[40] = {
				 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  // 0
				  0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  // 1
				  0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  // 2
				 -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  // 3
				 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  // 4
				  0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  // 5
				  0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  // 6
				 -0.5f,  0.5f,  0.5f,  1.0f,  0.0f   // 7
			};

			if (indexBuffer) {
				_indexBuffer = indexBuffer;
				_indexBuffer->grab();
			}
			else {
				_indexBuffer = new(std::nothrow) pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_16BIT);
				pxengine::PxeBuffer* indexData = new(std::nothrow) pxengine::PxeBuffer(sizeof(indices));
				memcpy(indexData->getBuffer(), indices, indexData->getSize());
				_indexBuffer->bufferData(*indexData);
				indexData->drop();
				indexData = nullptr;
			}

			if (vertextArray) {
				_vertexArray = vertextArray;
				_vertexArray->grab();
			}
			else {
				pxengine::PxeVertexBufferFormat format(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
				format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 2, false));
				pxengine::PxeVertexBuffer* vertexBuffer = new(std::nothrow) pxengine::PxeVertexBuffer(format);
				_vertexArray = new(std::nothrow) pxengine::PxeVertexArray();
				_vertexArray->addVertexBuffer(*vertexBuffer, 0, 0);
				_vertexArray->addVertexBuffer(*vertexBuffer, 1, 1);
				pxengine::PxeBuffer* vertexData = new(std::nothrow) pxengine::PxeBuffer(sizeof(vertices));
				memcpy(vertexData->getBuffer(), vertices, vertexData->getSize());
				vertexBuffer->bufferData(*vertexData);
				vertexData->drop();
				vertexData = nullptr;
				vertexBuffer->drop();
				vertexBuffer = nullptr;
			}

			physx::PxPhysics& physics = pxengine::pxeGetEngine().getPhysicsBase();
			if (shape) {
				shape->acquireReference();
				_pxShape = shape;
			}
			else {
				using namespace physx;
				using namespace pxengine;
				PxConvexMeshDesc meshDesc;
				meshDesc.points.count = 8;
				meshDesc.points.data = &vertices;
				meshDesc.points.stride = sizeof(float) * 5;
				meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;
				PxCooking& cooking = pxeGetEngine().getPhysicsCooking();
				PxConvexMesh* mesh = cooking.createConvexMesh(meshDesc, physics.getPhysicsInsertionCallback());
				PxMaterial* material = physics.createMaterial(1.0f, 0.5f, 0.5f);
				_pxShape = physics.createShape(PxConvexMeshGeometry(mesh), *material);
				mesh->release();
				material->release();
			}

			getPhysicsRigidActor()->attachShape(*_pxShape);
		}

		pxengine::PxeIndexBuffer* _indexBuffer;
		pxengine::PxeVertexArray* _vertexArray;
		physx::PxShape* _pxShape;
	};
}
#endif
