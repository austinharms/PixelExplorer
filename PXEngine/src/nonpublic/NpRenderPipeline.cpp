#include "NpRenderPipeline.h"

#include <new>
#include <list>

#include "NpLogger.h"
#include "NpFont.h"
#include "NpGuiGlBackend.h"
#include "NpGuiRenderMaterial.h"
#include "PxeFSHelpers.h"
#include "PxeShader.h"
#include "PxeRenderMaterialInterface.h"
#include "PxeRenderObjectInterface.h"

namespace pxengine {
	namespace nonpublic {
		NpRenderPipeline::NpRenderPipeline()
		{
			PxeShader* shader = loadShader(getAssetPath("shaders") / "pxengine_gui_shader.pxeshader");
			if (!shader) {
				PXE_FATAL("Failed to load gui shader");
			}

			NpGuiRenderMaterial* guiMaterial = new(std::nothrow) NpGuiRenderMaterial(*shader);
			shader->drop();
			if (!guiMaterial) {
				PXE_FATAL("Failed to allocate gui render material");
			}

			_guiBackend = new(std::nothrow) NpGuiGlBackend(*guiMaterial);
			if (!_guiBackend) {
				PXE_FATAL("Failed to allocate gui backend");
			}

			_guiRenderMaterial = guiMaterial;
			_vsyncMode = 0;
		}

		NpRenderPipeline::~NpRenderPipeline()
		{
			PXE_INFO("PxeRenderPipeline destroyed");
		}

		void NpRenderPipeline::initializePipeline() {
			glGenFramebuffers(1, &_framebuffer);
			glGenRenderbuffers(1, &_depthbuffer);
			PXE_INFO("PxeRenderPipeline initialized");
		}

		void NpRenderPipeline::uninitializePipeline()
		{
			glDeleteFramebuffers(1, &_framebuffer);
			glDeleteRenderbuffers(1, &_depthbuffer);
			_guiBackend->drop();
			_guiRenderMaterial->drop();
			PXE_INFO("PxeRenderPipeline uninitialized");
		}

		void NpRenderPipeline::renderScene(NpScene& scene, PxeRenderTexture& targetTexture, const glm::mat4& PVMatrix, bool renderGui)
		{
			if (targetTexture.getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
				PXE_WARN("Attempted to render to invalid PxeRenderTexture");
				return;
			}

			if (targetTexture.getWidth() == 0 || targetTexture.getHeight() == 0) {
				PXE_WARN("Attempted to render to PxeRenderTexture with invalid size");
				return;
			}

			glBindRenderbuffer(GL_RENDERBUFFER, _depthbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, targetTexture.getWidth(), targetTexture.getHeight());
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthbuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture.getGlTextureId(), 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
				scene.acquireObjectsReadLock();
				glViewport(0, 0, targetTexture.getWidth(), targetTexture.getHeight());
				glClearColor(0, 0, 0, 1);
				// Ensure we clear the full framebuffer
				glDisable(GL_SCISSOR_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				{
					PxeRenderMaterialInterface* currentMaterial = nullptr;
					PxeShader* currentShader = nullptr;
					int32_t mvpLocation = -1;
					int32_t camLocation = -1;
					const std::list<PxeRenderObjectInterface*>& renderList = scene.getRenderObjects(PxeRenderPass::FORWARD);
					if (!renderList.empty()) {
						for (PxeRenderObjectInterface* obj : renderList) {
							if (&(obj->getRenderMaterial()) != currentMaterial) {
								currentMaterial = &(obj->getRenderMaterial());
								if (&(currentMaterial->getShader()) != currentShader) {
									currentShader = &(currentMaterial->getShader());
									currentShader->bind();
									mvpLocation = currentShader->getUniformLocation("u_MVP");
									camLocation = currentShader->getUniformLocation("u_CAMPOS");
								}

								currentMaterial->applyMaterial();
							}

							currentShader->setUniformM4f(mvpLocation, PVMatrix * obj->getPositionMatrix());
							//currentShader->setUniform3f(camLocation, );
							obj->onRender();
						}
					}
				}

				if (renderGui) {
					_guiRenderMaterial->getShader().bind();
					_guiRenderMaterial->applyMaterial();
					ImGui::NewFrame();
					const std::list<PxeRenderObjectInterface*>& renderList = scene.getRenderObjects(PxeRenderPass::GUI);
					for (PxeRenderObjectInterface* obj : renderList) {
						obj->onRender();
					}

					ImGui::Render();
					_guiBackend->renderDrawData();
				}

				scene.releaseObjectsReadLock();
			}
			else {
				PXE_ERROR("Failed to create render framebuffer");
			}

			glUseProgram(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		PXE_NODISCARD PxeShader* NpRenderPipeline::loadShader(const std::filesystem::path& path)
		{
			std::lock_guard lock(_shaderCacheMutex);
			auto it = _shaderCache.find(path);
			if (it != _shaderCache.end()) {
				it->second->grab();
				return it->second;
			}
			else {
				NpShader* shader = new(std::nothrow) NpShader(path);
				if (!shader) {
					PXE_ERROR("Failed to allocate PxeShader");
					return nullptr;
				}

				shader->initializeAsset();
				_shaderCache.emplace(path, shader);
				return shader;
			}
		}

		void NpRenderPipeline::removeShaderFromCache(NpShader& shader)
		{
			std::lock_guard lock(_shaderCacheMutex);
			if (!_shaderCache.erase(shader.getShaderPath())) {
				PXE_ERROR("Attempted to remove shader " + shader.getShaderPath().string() + " from cache that was not in shader cache");
			}
		}

		PXE_NODISCARD PxeFont* NpRenderPipeline::loadFont(const std::filesystem::path& path, uint16_t baseSize, PxeFontScalingMode scaleMode, const glm::i32vec2& referenceWindowSize)
		{
			NpFontSource* fontSource = nullptr;

			{
				std::lock_guard lock(_fontSourceCacheMutex);
				auto cacheItr = _fontSourceCache.find(path);
				if (cacheItr != _fontSourceCache.end()) fontSource = cacheItr->second;
			}

			if (!fontSource) {
				fontSource = new(std::nothrow) NpFontSource(path);
				if (!fontSource) {
					PXE_ERROR("Failed to allocate PxeFont's source");
					return nullptr;
				}

				std::lock_guard lock(_fontSourceCacheMutex);
				_fontSourceCache.emplace(path, fontSource);
			}

			NpFont* font = new(std::nothrow) NpFont(*fontSource, baseSize, scaleMode, referenceWindowSize);
			if (!font) {
				PXE_ERROR("Failed to allocate PxeFont");
				return nullptr;
			}

			if (fontSource->getRequiredSize() < baseSize)
				fontSource->setRequiredSize(baseSize);
			return font;
		}

		void NpRenderPipeline::removeFontSourceFromCache(NpFontSource& source)
		{
			std::lock_guard lock(_fontSourceCacheMutex);
			if (!_fontSourceCache.erase(source.getSourcePath())) {
				PXE_ERROR("Attempted to remove font source " + source.getSourcePath().string() + " from cache that was not in font source cache");
			}
		}

		void NpRenderPipeline::installGuiBackend()
		{
			_guiBackend->grab();
			_guiBackend->installBackend();
		}

		void NpRenderPipeline::uninstallGuiBackend()
		{
			_guiBackend->uninstallBackend();
			_guiBackend->drop();
		}

		void NpRenderPipeline::rebuildGuiFontAtlas()
		{
			if (_guiBackend->getAssetStatus() != PxeGLAssetStatus::INITIALIZED) return;
			bool updateRequired = false;
			std::lock_guard lock(_fontSourceCacheMutex);
			for (auto sourcePair : _fontSourceCache) {
				if (sourcePair.second->fontAtlasUpdateRequired()) {
					updateRequired = true;
					break;
				}
			}

			if (!updateRequired && _guiFontAtlas.IsBuilt()) return;
			_guiFontAtlas.Clear();
			_guiFontAtlas.AddFontDefault();
			for (auto sourcePair : _fontSourceCache)
				sourcePair.second->addFontToAtlas(_guiFontAtlas);
			_guiBackend->rebuildFontTexture();
			PXE_INFO("Rebuilt gui font atlas");
		}

		PXE_NODISCARD ImFontAtlas* NpRenderPipeline::getFontAtlas()
		{
			return &_guiFontAtlas;
		}

		void NpRenderPipeline::setVSyncMode(int8_t mode)
		{
			_vsyncMode = mode;
		}

		PXE_NODISCARD int8_t NpRenderPipeline::getVSyncMode() const
		{
			return _vsyncMode;
		}

		PXE_NODISCARD PxeRenderMaterialInterface* NpRenderPipeline::getGuiRenderMaterial() const
		{
			return _guiRenderMaterial;
		}
	}
}