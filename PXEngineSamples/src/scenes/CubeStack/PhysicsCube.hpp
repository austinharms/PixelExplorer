#ifndef PXENGINESAMPLES_PHYSICS_RENDERABLE_H_
#define PXENGINESAMPLES_PHYSICS_RENDERABLE_H_
#include <mutex>

#include "PxeEngineAPI.h"
#include "PxPhysicsAPI.h"
#include "PxShape.h"
#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "../shared/RenderData.h"

class PhysicsCube : public pxengine::PxeDynamicPhysicsRenderObject
{
public:
	static PhysicsCube* createPhysicsCube(const glm::vec3& pos, bool dynamic, pxengine::PxeRenderMaterialInterface& material, const glm::vec3& velocity = glm::vec3(0)) {
		std::lock_guard lock(s_mutex);
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
		bool createdVAO = false;
		bool createdShape = false;
		if (!s_pxShape) {
			using namespace physx;
			PxPhysics& physics = pxengine::pxeGetEngine().getPhysicsBase();
			PxCooking& cooking = pxengine::pxeGetEngine().getPhysicsCooking();
			PxConvexMeshDesc meshDesc;
			meshDesc.points.count = 8;
			meshDesc.points.data = &renderCubeVertices;
			meshDesc.points.stride = sizeof(float) * 5;
			meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;
			PxConvexMesh* mesh = cooking.createConvexMesh(meshDesc, physics.getPhysicsInsertionCallback());
			PxMaterial* material = physics.createMaterial(1.0f, 0.5f, 0.5f);
			s_pxShape = physics.createShape(PxConvexMeshGeometry(mesh), *material);
			mesh->release();
			material->release();
			createdShape = true;

			if (!s_pxShape) {
				actor->release();
				return nullptr;
			}
		}

		if (!s_vertexArray) {
			// Create and load vertex buffer
			pxengine::PxeVertexBufferFormat format;
			format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 3, false));
			format.addAttrib(pxengine::PxeVertexBufferAttrib(pxengine::PxeVertexBufferAttribType::FLOAT, 2, false));
			pxengine::PxeVertexBuffer* vertexBuffer = new pxengine::PxeVertexBuffer(format);
			pxengine::PxeBuffer* vertexData = new pxengine::PxeBuffer(sizeof(renderCubeVertices));
			memcpy(vertexData->getBuffer(), renderCubeVertices, vertexData->getSize());
			vertexBuffer->bufferData(*vertexData);
			vertexData->drop();

			// Create and load index buffer
			pxengine::PxeIndexBuffer* indexBuffer = new pxengine::PxeIndexBuffer(pxengine::PxeIndexType::UNSIGNED_8BIT);
			pxengine::PxeBuffer* indexData = new pxengine::PxeBuffer(sizeof(renderCubeIndices));
			memcpy(indexData->getBuffer(), renderCubeIndices, indexData->getSize());
			indexBuffer->bufferData(*indexData);
			indexData->drop();

			// Attach buffers to VAO
			s_vertexArray = new pxengine::PxeVertexArray();
			s_vertexArray->addVertexBuffer(*vertexBuffer, 0, 0);
			s_vertexArray->addVertexBuffer(*vertexBuffer, 1, 1);
			s_vertexArray->setIndexBuffer(indexBuffer);
			vertexBuffer->drop();
			indexBuffer->drop();
			createdVAO = true;
		}

		PhysicsCube* renderable = new PhysicsCube(*actor, material);
		if (createdVAO) s_vertexArray->drop();
		if (createdShape) s_pxShape->release();
		renderable->positionMatrix = glm::translate(renderable->positionMatrix, pos);
		return renderable;
	}

	virtual ~PhysicsCube() {
		std::lock_guard lock(s_mutex);
		if (s_vertexArray->getRefCount() == 1) {
			s_vertexArray->drop();
			s_vertexArray = nullptr;
		}
		else {
			s_vertexArray->drop();
		}

		// PxeDynamicPhysicsRenderObject will release the actor holding the shape
		// if it is the last one, clear the shape
		if (s_pxShape->getReferenceCount() == 1) {
			s_pxShape = nullptr;
		}
	}

	void onRender() override {
		s_vertexArray->bind();
		glDrawElements(GL_TRIANGLES, s_vertexArray->getIndexBuffer()->getIndexCount(), (uint32_t)s_vertexArray->getIndexBuffer()->getIndexType(), nullptr);
	}

private:
	PhysicsCube(physx::PxRigidActor& actor, pxengine::PxeRenderMaterialInterface& material) : pxengine::PxeDynamicPhysicsRenderObject(material, &actor) {
		s_vertexArray->grab();
		getPhysicsRigidActor()->attachShape(*s_pxShape);
	}

	static std::mutex s_mutex;
	static pxengine::PxeVertexArray* s_vertexArray;
	static physx::PxShape* s_pxShape;
};

pxengine::PxeVertexArray* PhysicsCube::s_vertexArray = nullptr;
physx::PxShape* PhysicsCube::s_pxShape = nullptr;
std::mutex PhysicsCube::s_mutex;
#endif
