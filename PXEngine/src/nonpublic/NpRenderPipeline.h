#ifndef PXENGINE_NONPUBLIC_RENDER_PIPELINE_H_
#define PXENGINE_NONPUBLIC_RENDER_PIPELINE_H_
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "PxeRenderPipeline.h"
#include "PxeRenderTexture.h"
#include "NpShader.h"
#include "NpScene.h"
#include "NpFontSource.h"
#include "imgui.h"

namespace pxengine {
	namespace nonpublic {
		class NpGuiGlBackend;
		class NpEngine;

		class NpRenderPipeline : public PxeRenderPipeline
		{
		public:
			//############# PxeRenderPipeline API ##################

			PXE_NODISCARD PxeShader* loadShader(const std::filesystem::path& path) override;
			PXE_NODISCARD PxeFont* loadFont(const std::filesystem::path& path, uint16_t baseSize, PxeFontScalingMode scaleMode, const glm::i32vec2& referenceWindowSize) override;
			void setVSyncMode(int8_t mode) override;
			PXE_NODISCARD int8_t getVSyncMode() const override;
			PXE_NODISCARD PxeRenderMaterialInterface* getGuiRenderMaterial() const override;


			//############# PRIVATE API ##################

			NpRenderPipeline();
			virtual ~NpRenderPipeline();
			// Note: if renderGui is true this method expects a valid gui context to be bound and ready for drawing
			void renderScene(NpScene& scene, PxeRenderTexture& targetTexture, const glm::mat4& PVMatrix, bool renderGui);
			void removeShaderFromCache(NpShader& shader);
			void removeFontSourceFromCache(NpFontSource& source);
			void installGuiBackend();
			void uninstallGuiBackend();
			void rebuildGuiFontAtlas();
			PXE_NODISCARD ImFontAtlas* getFontAtlas();

		private:
			friend class NpEngine;
			void initializePipeline();
			void uninitializePipeline();

			NpGuiGlBackend* _guiBackend;
			PxeRenderMaterialInterface* _guiRenderMaterial;

			std::mutex _shaderCacheMutex;
			std::unordered_map<std::filesystem::path, NpShader*> _shaderCache;

			std::mutex _fontSourceCacheMutex;
			std::unordered_map<std::filesystem::path, NpFontSource*> _fontSourceCache;

			ImFontAtlas _guiFontAtlas;
			int8_t _vsyncMode;

			uint32_t _framebuffer;
			uint32_t _depthbuffer;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_RENDER_PIPELINE_H_
