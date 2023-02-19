#ifndef PXENGINE_NONPUBLIC_GUI_OPENGL_BACKEND_H_
#define PXENGINE_NONPUBLIC_GUI_OPENGL_BACKEND_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "NpGuiRenderMaterial.h"

namespace pxengine {
	namespace nonpublic {
		class NpGuiGlBackend : public PxeGLAsset
		{
		public:
			NpGuiGlBackend(NpGuiRenderMaterial& guiMaterial);
			virtual ~NpGuiGlBackend();

            // installs the gui backend to the current gui context
            void installBackend();

            // removes the gui backend from the current gui context
            void uninstallBackend();

            void rebuildFontTexture();
            void renderDrawData();

        protected:
            void initializeGl() override;
            void uninitializeGl() override;

		private:
            NpGuiRenderMaterial& _guiMaterial;
            uint32_t _glFontTextureId;
            uint32_t _glVertexArray;
            uint32_t _glArrayBuffer;
            uint32_t _glElementBuffer;
		};
	}
}

#endif // !PXENGINE_NONPUBLIC_GUI_OPENGL_BACKEND_H_
