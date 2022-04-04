#ifndef PHYSICSSCENE_H
#define PHYSICSSCENE_H

#include "PhysicsObject.h"
#include "PhysicsBase.h"
#include "PxPhysicsAPI.h"
#include "RefCounted.h"
namespace px::physics {
	class PhysicsScene : public RefCounted {
	public:
		const static float FIXED_STEP;
		PhysicsScene();
		virtual ~PhysicsScene();
		void insertObject(PhysicsObject* obj);
		void removeObject(PhysicsObject* obj);
		void simulate(float time);

	private:
		physx::PxScene* _pxScene;
		float _accumulator;

	};
}  // namespace px::physics
#endif