#ifndef PXENGINE_RENDER_PROPERTIES_H_
#define PXENGINE_RENDER_PROPERTIES_H_
#include "PxeRefCount.h"
#include "PxeShader.h"

namespace pxengine {
	// Class that stores properties to apply when rendering the object
	// TODO Add implementation to file
	class PxeRenderProperties : public PxeRefCount
	{
	public:
		PXE_NODISCARD PxeShader* getShader() const;
		virtual ~PxeRenderProperties();
		PXE_NOCOPY(PxeRenderProperties);

	protected:
		PxeRenderProperties(PxeShader* shader = nullptr);
		void setShader(PxeShader* shader);

	private:
		PxeShader* _renderShader;
	};
}
#endif // !PXENGINE_RENDER_PROPERTIES_H_