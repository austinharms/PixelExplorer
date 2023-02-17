#ifndef PXENGINE_NONPUBLIC_RENDER_COLLECTION_H_
#define PXENGINE_NONPUBLIC_RENDER_COLLECTION_H_
#include <list>

#include "PxeTypes.h"
#include "PxeObject.h"
#include "PxeRenderObjectInterface.h"

namespace pxengine {
	namespace nonpublic {
		class NpRenderPipeline;
		class NpScene;

		class NpRenderCollection
		{
		public:
			NpRenderCollection(NpScene* scene);
			~NpRenderCollection();
			void addObject(PxeObject& obj);
			void removeObject(PxeObject& obj);
			void clear();

		private:
			friend class NpRenderPipeline;
			std::list<PxeRenderObjectInterface*> _renderLists[PxeRenderPassCount];
			NpScene* _scene;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_RENDER_COLLECTION_H_
