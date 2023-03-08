#ifndef PXENGINE_GUI_RENDER_PROPERTIES_H_
#define PXENGINE_GUI_RENDER_PROPERTIES_H_
PXE_PRIVATE_IMPLEMENTATION_START
#include <mutex>
PXE_PRIVATE_IMPLEMENTATION_END
#include "PxeRenderProperties.h"

namespace pxengine {
	class PxeGuiRenderProperties : public PxeRenderProperties
	{
	public:
		// Returns a PxeGuiRenderProperties instance or nullptr on failure
		static PxeGuiRenderProperties* getInstance();
		virtual ~PxeGuiRenderProperties();
		PXE_NOCOPY(PxeGuiRenderProperties);

	protected:
		virtual void onDelete() override;

	private:
		PxeGuiRenderProperties();
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		static PxeGuiRenderProperties* s_propertiesInstance;
		static std::mutex s_instanceMutex;
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_GUI_RENDER_PROPERTIES_H_
