#ifndef PXENGINE_GUI_OBJECT_H_
#define PXENGINE_GUI_OBJECT_H_
#include "PxeTypes.h"
#include "PxeObject.h"
#include "PxeRenderObjectInterface.h"
#include "PxeEngine.h"
#include "PxeRenderMaterialInterface.h"
#include "PxeRenderPipeline.h"

namespace pxengine {
	// base class for GUI rendering
	class PxeGUIObject : public PxeObject, public PxeRenderObjectInterface
	{
	public:
		inline PxeGUIObject(PxeObjectFlags flags = PxeObjectFlags::RENDER_OBJECT) : PxeObject(flags), PxeRenderObjectInterface(*pxeGetEngine().getRenderPipeline().getGuiRenderMaterial()) {}
		virtual ~PxeGUIObject() = default;

		// This method should never be called as this is a gui object
		PXE_NODISCARD const glm::mat4& getPositionMatrix() const override { return glm::mat4(1); }

		// This method should draw GUI objects to the current GUI context
		// Note: you can assume a valid ImGui context and frame
		// Note: this requires the RENDER_OBJECT flag to be set (flag is set by default)
		virtual void onRender() override = 0;
	};
}
#endif // !PXENGINE_GUI_OBJECT_H_
