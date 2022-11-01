#include "PxeRefCount.h"

#ifndef PXENGINE_GLASSET_H_
#define PXENGINE_GLASSET_H_
namespace pxengine::rendering {
	class PxeGLAsset : public PxeRefCount
	{
	public:
		PxeGLAsset();
		virtual ~PxeGLAsset();
		virtual void bind() = 0;
		virtual void unbind() = 0;

	protected:
		const bool getInitializedFlag();
		void setInitializedFlag();
		virtual void deleteAsset() = 0;

	private: 
		bool _initialized;

		void onDelete() override;
	};
}
#endif // !PXENGINE_GLASSET_H_
