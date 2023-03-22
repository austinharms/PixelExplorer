#ifndef PXENGINE_GUI_RENDER_PROPERTIES_H_
#define PXENGINE_GUI_RENDER_PROPERTIES_H_
#include "PxeRenderProperties.h"

namespace pxengine {
	class PxeGuiRenderProperties : public PxeRenderProperties
	{
	public:
		// Returns a PxeGuiRenderProperties instance or nullptr on failure
		static PxeGuiRenderProperties* create();
		PXE_NODISCARD PxeShader* getShader() const override;
		virtual ~PxeGuiRenderProperties();
		PXE_NOCOPY(PxeGuiRenderProperties);

	protected:
		void onApplyProperties() override;

	private:
		PxeGuiRenderProperties(PxeShader& shader);
		PxeShader& _guiShader;
	};
}
#endif // !PXENGINE_GUI_RENDER_PROPERTIES_H_
