#include "StaticPhysicsObject.h"

#include "PhysicsBase.h"
#include "PhysicsScene.h"
namespace px::physics {
	StaticPhysicsObject::StaticPhysicsObject(const glm::vec3& position,
		util::DataBuffer<float>* vertices,
		util::DataBuffer<uint32_t>* indices,
		const float vertexStride,
		PhysicsScene* scene) {
		_scene = nullptr;
		_pxShape = nullptr;
		physx::PxPhysics* phys = PhysicsBase::getInstance().getPxPhysics();
		_pxStaticBody = phys->createRigidStatic(
			physx::PxTransform(position.x, position.y, position.z));
		_pxStaticBody->userData = this;
		updateMesh(vertices, indices, vertexStride);
		setScene(scene);
	}

	void StaticPhysicsObject::updateMesh(util::DataBuffer<float>* vertices,
		util::DataBuffer<uint32_t>* indices,
		const float vertexStride) {
		if (_pxShape != nullptr) {
			_pxStaticBody->detachShape(*_pxShape);
			_pxShape->release();
		}

		if (vertices != nullptr && indices != nullptr) {
			indices->grab();
			vertices->grab();
			physx::PxTriangleMeshDesc meshDesc;
			vertices->makeReadOnly();
			meshDesc.points.count = vertices->length / vertexStride;
			meshDesc.points.stride = sizeof(float) * vertexStride;
			meshDesc.points.data = vertices->buffer;

			indices->makeReadOnly();
			meshDesc.triangles.count = indices->length / 3;
			meshDesc.triangles.stride = sizeof(uint32_t) * 3;
			meshDesc.triangles.data = indices->buffer;
			physx::PxPhysics* phys = PhysicsBase::getInstance().getPxPhysics();
			physx::PxCooking* cook = PhysicsBase::getInstance().getPxCooking();
			physx::PxTriangleMesh* mesh =
				cook->createTriangleMesh(meshDesc, phys->getPhysicsInsertionCallback());
			physx::PxMeshScale scale(physx::PxVec3(1));
			const physx::PxTriangleMeshGeometry geom(mesh, scale);
			_pxShape = phys->createShape(
				geom, *PhysicsBase::getInstance().getDefaultPxMaterial(), true);
			_pxStaticBody->attachShape(*_pxShape);
			mesh->release();
			indices->makeWriteable();
			indices->drop();
			vertices->makeWriteable();
			vertices->drop();
		}
	}

	void StaticPhysicsObject::setScene(PhysicsScene* scene) {
		if (_scene != nullptr) {
			_scene->removeObject(this);
			_scene->drop();
		}

		_scene = scene;
		if (_scene != nullptr) {
			_scene->grab();
			_scene->insertObject(this);
		}
	}

	PhysicsScene* StaticPhysicsObject::getScene() const { return _scene; }

	StaticPhysicsObject::~StaticPhysicsObject() {
		if (_scene != nullptr) {
			_scene->removeObject(this);
			_scene->drop();
		}

		_pxStaticBody->release();
		if (_pxShape != nullptr)
			_pxShape->release();
	}

	physx::PxActor* StaticPhysicsObject::getPxActor() const {
		return _pxStaticBody;
	}
}  // namespace px::physics