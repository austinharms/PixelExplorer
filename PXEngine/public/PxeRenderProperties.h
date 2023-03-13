#ifndef PXENGINE_RENDER_PROPERTIES_H_
#define PXENGINE_RENDER_PROPERTIES_H_
#include "PxeRefCount.h"
#include "PxeShader.h"

namespace pxengine {
	class PxeRenderPipeline;

	// Class that stores properties to apply when rendering the object
	class PxeRenderProperties : public PxeRefCount
	{
	public:
		PXE_NODISCARD virtual PxeShader* getShader() const = 0;
		virtual ~PxeRenderProperties() = default;

	protected:
		PxeRenderProperties() = default;
		// this should apply the properties to the shader
		// Note: you can assume the PxeShader returned by getShader() is bound
		virtual void onApplyProperties() = 0;
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeRenderPipeline;
		void apply();
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_RENDER_PROPERTIES_H_