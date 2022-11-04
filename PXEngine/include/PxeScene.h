#include "PxeRefCount.h"

#ifndef PXENGINE_SCENE_H_
#define PXENGINE_SCENE_H_
namespace pxengine {
	class PxeScene : public PxeRefCount
	{
	public:
		virtual ~PxeScene() {}

		// simuates {time} seconds for all the physics bodies in the scene 
		virtual void simulatePhysics(float time) = 0;


	};
}
#endif // !PXENGINE_SCENE_H_
