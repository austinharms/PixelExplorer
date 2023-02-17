#ifndef PXENGINE_NONPUBLIC_RENDER_PIPELINE_H_
#define PXENGINE_NONPUBLIC_RENDER_PIPELINE_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "NpRenderCollection.h"

namespace pxengine {
	namespace nonpublic {
		class NpRenderPipeline : public PxeGLAsset
		{
		public:
			//############# PxeGLAsset API ##################

			void bind() override;
			void unbind() override;


			//############# PRIVATE API ##################

			NpRenderPipeline();
			virtual ~NpRenderPipeline();
			void render()

		protected:
			//############# PxeGLAsset API ##################

			void initializeGl() override;
			void uninitializeGl() override;

		private:

		};
	}
}
#endif // !PXENGINE_NONPUBLIC_RENDER_PIPELINE_H_
