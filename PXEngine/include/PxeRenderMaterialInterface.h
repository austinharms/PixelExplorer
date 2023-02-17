#ifndef PXENGINE_RENDER_MATERIAL_INTERFACE_H_
#define PXENGINE_RENDER_MATERIAL_INTERFACE_H_
#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeShader.h"

namespace pxengine {
	// Base interface class for all render materials
	// Note: a render material can only be used with a single shader
	class PxeRenderMaterialInterface : public PxeRefCount
	{
	public:
		PxeRenderMaterialInterface(PxeRenderPass renderPass) : _renderPass(renderPass) {}
		virtual ~PxeRenderMaterialInterface() = default;

		// Apply all required OpenGl state changes to use material
		// Note: you can assume a valid OpenGl context
		// Note: you can assume the shader returned by getShader is bound
		virtual void applyMaterial() = 0;

		// Returns the shader this material should be used with
		// Note: the returned shader must never change
		PXE_NODISCARD virtual PxeShader& getShader() const = 0;

		PXE_NODISCARD inline PxeRenderPass getRenderPass() const { return _renderPass; }

		PxeRenderMaterialInterface(const PxeRenderMaterialInterface& other) = delete;
		PxeRenderMaterialInterface operator=(const PxeRenderMaterialInterface& other) = delete;

	private:
		const PxeRenderPass _renderPass;
	};
}
#endif // !PXENGINE_RENDER_MATERIAL_INTERFACE_H_
