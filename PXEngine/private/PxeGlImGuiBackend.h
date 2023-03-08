#ifndef PXENGINE_OPENGL_IMGUI_BACKEND_H_
#define PXENGINE_OPENGL_IMGUI_BACKEND_H_
#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeGuiShader.h"
#include "PxeGuiRenderProperties.h"

namespace pxengine {
        class PxeGlImGuiBackend : public PxeGLAsset
        {
        public:
            // installs the gui backend to the current gui context
            void installBackend();

            // removes the gui backend from the current gui context
            void uninstallBackend();

            void rebuildFontTexture();
            void renderDrawData();

            PXE_NODISCARD PxeGuiRenderProperties* getGuiRenderProperties();

            PxeGlImGuiBackend();
            virtual ~PxeGlImGuiBackend();
            PXE_NOCOPY(PxeGlImGuiBackend);

        protected:
            void initializeGl() override;
            void uninitializeGl() override;

        private:
            PxeGuiShader* _guiShader;
            PxeGuiRenderProperties* _guiRenderProperties;
            uint32_t _glFontTextureId;
            uint32_t _glVertexArray;
            uint32_t _glArrayBuffer;
            uint32_t _glElementBuffer;
        };
}
#endif // !PXENGINE_OPENGL_IMGUI_BACKEND_H_
