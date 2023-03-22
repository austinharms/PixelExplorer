#include "PxeRenderPipeline.h"

#include <unordered_map>
#include <mutex>
#include <string>
#include <list>

#include "imgui.h"
#include "PxeFontFile.h"
#include "PxeLogger.h"
#include "GL/glew.h"
#include "PxeRenderComponent.h"
#include "PxeScene.h"
#include "PxeRenderProperties.h"
#include "PxeGuiRenderProperties.h"

namespace pxengine {
	struct PxeRenderPipeline::Impl {
		PxeGlImGuiBackend* _guiBackend;
		PxeGuiRenderProperties* _guiProperties;
		uint32_t _framebuffer;
		uint32_t _depthbuffer;
		std::mutex _shadersMutex;
		std::mutex _fontFilesMutex;
		std::unordered_map<PxeShaderId, PxeShader*> _shaders;
		std::unordered_map<std::filesystem::path, PxeFontFile*> _fontFiles;
		ImFontAtlas _fontAtlas;
		int8_t _vsyncMode;
		uint8_t _renderDepth;
		uint8_t _maxRenderDepth;
	};

	PxeRenderPipeline::PxeRenderPipeline()
	{
		static_assert(sizeof(Impl) <= STORAGE_SIZE, "PxeRenderPipeline::STORAGE_SIZE need be changed");
		static_assert(STORAGE_ALIGN == alignof(Impl), "PxeRenderPipeline::STORAGE_ALIGN need be changed");

		new(&_storage) Impl();
		Impl& imp = impl();
		imp._guiBackend = nullptr;
		imp._guiProperties = nullptr;
		imp._framebuffer = 0;
		imp._depthbuffer = 0;
		imp._vsyncMode = 0;
		imp._renderDepth = 0;
		imp._maxRenderDepth = 1;
	}

	PxeRenderPipeline::~PxeRenderPipeline()
	{
		impl().~Impl();
	}

	PXE_NODISCARD PxeShader* PxeRenderPipeline::getShader(PxeShaderId shaderId)
	{
		Impl& imp = impl();
		auto shaderItr = imp._shaders.find(shaderId);
		if (shaderItr != imp._shaders.end()) return shaderItr->second;
		return nullptr;
	}

	void PxeRenderPipeline::addShader(PxeShaderId shaderId, PxeShader* shader)
	{
		if (!shader) {
			PXE_ERROR("Failed to allocate PxeShader");
			return;
		}

		impl()._shaders.emplace(shaderId, shader);
	}

	void PxeRenderPipeline::lockShaders()
	{
		impl()._shadersMutex.lock();
	}

	void PxeRenderPipeline::unlockShaders()
	{
		impl()._shadersMutex.unlock();
	}

	PXE_NODISCARD PxeFont* PxeRenderPipeline::loadFont(const std::filesystem::path& path, uint16_t baseSize, PxeFontScalingMode scaleMode, const glm::i32vec2& referenceWindowSize)
	{
		Impl& imp = impl();
		PxeFontFile* fontFile = nullptr;

		{
			std::lock_guard lock(imp._fontFilesMutex);
			auto fileItr = imp._fontFiles.find(path);
			if (fileItr != imp._fontFiles.end()) fontFile = fileItr->second;

			if (!fontFile) {
				fontFile = new(std::nothrow) PxeFontFile(path);
				if (!fontFile) {
					PXE_ERROR("Failed to load font, failed to allocate PxeFontFile");
					return nullptr;
				}

				imp._fontFiles.emplace(path, fontFile);
			}

			fontFile->grab();
		}

		PxeFont* font = new(std::nothrow) PxeFont(*fontFile, baseSize, scaleMode, referenceWindowSize);
		fontFile->drop();
		if (!font) {
			PXE_ERROR("Failed to load font, failed to allocate PxeFont");
			return nullptr;
		}

		if (fontFile->getRequestedSize() < baseSize)
			fontFile->setRequestedSize(baseSize);
		return font;
	}

	void PxeRenderPipeline::setVSyncMode(int8_t mode)
	{
		impl()._vsyncMode = mode;
	}

	PXE_NODISCARD int8_t PxeRenderPipeline::getVSyncMode() const
	{
		return impl()._vsyncMode;
	}

	PXE_NODISCARD PxeRenderProperties& PxeRenderPipeline::getGuiRenderProperties()
	{
		Impl& imp = impl();
		if (imp._guiProperties) return *imp._guiProperties;
		imp._guiProperties = PxeGuiRenderProperties::create();
		if (!imp._guiProperties) {
			PXE_FATAL("Failed to create PxeGuiRenderProperties");
		}

		return *imp._guiProperties;
	}

	void PxeRenderPipeline::render(PxeRenderTarget& renderTarget)
	{
		Impl& imp = impl();
		if (++imp._renderDepth > imp._maxRenderDepth) {
			PXE_WARN("Max render depth exceeded skipping render");
			--imp._renderDepth;
			return;
		}

		if (!renderTarget.getRenderTargetValid()) return;
		renderTarget.onPrepareForRender();
		PxeScene& scene = *renderTarget.getRenderScene();
		scene.grab();
		const std::list<PxeRenderComponent*>& renderObjects = scene.acquireRenderComponents();
		auto renderItr = renderObjects.begin();
		PxeShader* curShader = nullptr;
		PxeRenderProperties* curProperties = nullptr;
		while (renderItr != renderObjects.end())
		{
			PxeRenderComponent& component = **renderItr;
			PxeRenderProperties& properties = component.getRenderProperties();
			if (curProperties != &properties) {
				if (!properties.getShader()) {
					++renderItr;
					continue;
				}

				PxeShader& shader = *properties.getShader();
				if (&shader != curShader) {
					if (shader.getExecutionOrder() >= PxeDefaultShaderExecutionOrder::PRE_RENDER) break;
					if (curShader) curShader->unbind();
					shader.bind();
					shader.setRenderTarget(renderTarget);
					if (curShader && curShader->getExecutionOrder() > shader.getExecutionOrder()) {
						PXE_ERROR("PxeScene's PxeRenderComponent list out of order, render results may be incorrect");
					}

					curShader = &shader;
				}

				properties.apply();
				curProperties = &properties;
			}
			
			curShader->setObjectTransform(component.getParentObject()->getTransform());
			component.onRender();
			++renderItr;
		}

		int32_t width = renderTarget.getRenderWidth();
		int32_t height = renderTarget.getRenderHeight();
		renderTarget.getRenderCamera()->setWindowSize(width, height);
		renderTarget.getRenderOutputTexture()->resize(width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, imp._depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, imp._framebuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, imp._depthbuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget.getRenderOutputTexture()->getGlTextureId(), 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			PXE_ERROR("Failed to render PxeRenderTarget, failed to create GL_FRAMEBUFFER");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			scene.drop();
			return;
		}

		glViewport(0, 0, width, height);
		glClearColor(0, 0, 0, 1);
		// Ensure we clear the full framebuffer
		glDisable(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		while (renderItr != renderObjects.end())
		{
			PxeRenderComponent& component = **renderItr;
			PxeRenderProperties& properties = component.getRenderProperties();
			if (curProperties != &properties) {
				if (!properties.getShader()) {
					++renderItr;
					continue;
				}

				PxeShader& shader = *(properties.getShader());
				if (&shader != curShader) {
					if (curShader) curShader->unbind();
					shader.bind();
					shader.setRenderTarget(renderTarget);
					if (curShader && curShader->getExecutionOrder() > shader.getExecutionOrder()) {
						PXE_ERROR("PxeScene's PxeRenderComponent list out of order, render results may be incorrect");
					}

					curShader = &shader;
				}

				properties.apply();
				curProperties = &properties;
			}

			curShader->setObjectTransform(component.getParentObject()->getTransform());
			component.onRender();
			++renderItr;
		}

		if (curShader) curShader->unbind();
		scene.releaseRenderComponents();
		scene.drop();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		renderTarget.onRenderCompleted();
		--imp._renderDepth;
	}

	void PxeRenderPipeline::removeShader(PxeShader& shader)
	{
		Impl& imp = impl();
		std::lock_guard lock(imp._shadersMutex);
		if (!imp._shaders.erase(shader.getShaderId())) {
			PXE_ERROR("Attempted to remove uncached PxeShader with id " + std::to_string(shader.getShaderId()) + " from cache");
		}
	}

	void PxeRenderPipeline::removeFontFile(PxeFontFile& file)
	{
		Impl& imp = impl();
		std::lock_guard lock(imp._fontFilesMutex);
		if (!imp._fontFiles.erase(file.getPath())) {
			PXE_ERROR("Attempted to remove uncached PxeFontFile " + file.getPath().string() + " from cache");
		}
	}

	void PxeRenderPipeline::updateFontAtlas()
	{
		Impl& imp = impl();
		if (imp._guiBackend->getAssetStatus() != PxeGLAssetStatus::INITIALIZED) return;
		bool updateRequired = false;
		std::lock_guard lock(imp._fontFilesMutex);
		for (auto filePair : imp._fontFiles) {
			if (filePair.second->getAtlasUpdateRequested()) {
				updateRequired = true;
				break;
			}
		}

		if (!updateRequired && imp._fontAtlas.IsBuilt()) return;
		imp._fontAtlas.Clear();
		imp._fontAtlas.AddFontDefault();
		for (auto filePair : imp._fontFiles)
			filePair.second->addFontToAtlas(imp._fontAtlas);
		imp._guiBackend->rebuildFontTexture();
		PXE_INFO("Built font atlas");
	}

	PXE_NODISCARD PxeGlImGuiBackend* PxeRenderPipeline::getGuiBackend()
	{
		return impl()._guiBackend;
	}

	PXE_NODISCARD ImFontAtlas* PxeRenderPipeline::getFontAtlas()
	{
		return &impl()._fontAtlas;
	}

	void PxeRenderPipeline::initializePipeline()
	{
		Impl& imp = impl();
		glGenFramebuffers(1, &imp._framebuffer);
		glGenRenderbuffers(1, &imp._depthbuffer);
		imp._guiBackend = new(std::nothrow) PxeGlImGuiBackend();
		if (!imp._guiBackend) {
			PXE_FATAL("Failed to allocate PxeGlImGuiBackend");
		}

		// Ensure the gui PxeGuiRenderProperties are created by this point
		std::ignore = getGuiRenderProperties();
		PXE_INFO("PxeRenderPipeline initialized");
	}

	void PxeRenderPipeline::uninitializePipeline()
	{
		Impl& imp = impl();
		glDeleteFramebuffers(1, &imp._framebuffer);
		glDeleteRenderbuffers(1, &imp._depthbuffer);
		imp._guiBackend->drop();
		imp._guiProperties->drop();
		PXE_INFO("PxeRenderPipeline uninitialized");
	}
}