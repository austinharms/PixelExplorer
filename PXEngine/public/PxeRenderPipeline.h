#ifndef PXENGINE_RENDER_PIPELINE_H_
#define PXENGINE_RENDER_PIPELINE_H_
#include <filesystem>
#include <new>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeShader.h"
#include "PxeFont.h"
#include "PxeRenderTarget.h"
PXE_PRIVATE_IMPLEMENTATION_START
#include "PxeGlImGuiBackend.h"
PXE_PRIVATE_IMPLEMENTATION_END

namespace pxengine {
	class PxeEngine;

	// TODO Comment file
	class PxeRenderPipeline
	{
	public:
		// Loads a font file from {path} and returns a new PxeFont instance or nullptr on failure
		// Note: this only supports loading .ttf font files
		PXE_NODISCARD PxeFont* loadFont(const std::filesystem::path& path, uint16_t baseSize, PxeFontScalingMode scaleMode = PxeFontScalingMode::SMALLEST, const glm::i32vec2& referenceWindowSize = glm::i32vec2(600, 400));

		// Set VSync mode for all windows
		// Modes:
		//	0 for immediate updates
		//	1 for updates synchronized with
		//	-1 for adaptive VSync
		// Note: the default is 0
		void setVSyncMode(int8_t mode);

		// Returns VSync mode for all windows
		PXE_NODISCARD int8_t getVSyncMode() const;

		// Creates and returns PxeShader or nullptr on failure 
		// Note: all PxeShader are cached by PxeShaderId
		template<class T, typename std::enable_if<std::is_base_of<PxeShader, T>::value>::type* = nullptr>
		PXE_NODISCARD inline T* getShader()
		{
			PxeShaderId id = PxeShader::getShaderId<T>();
			PxeShader* shader;
			lockShaders();
			if ((shader = getShader(id))) {
				shader->grab();
				unlockShaders();
				return static_cast<T*>(shader);
			}

			shader = new(std::nothrow) T();
			addShader(id, shader);
			unlockShaders();
			return static_cast<T*>(shader);
		}

		~PxeRenderPipeline();
		PXE_NOCOPY(PxeRenderPipeline);

	private:
		PxeRenderPipeline();
		PXE_NODISCARD PxeShader* getShader(PxeShaderId shaderId);
		void addShader(PxeShaderId shaderId, PxeShader* shader);
		void lockShaders();
		void unlockShaders();

		static constexpr PxeSize STORAGE_SIZE = 1528;
		static constexpr PxeSize STORAGE_ALIGN = 8;
		std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
		PXE_PRIVATE_IMPLEMENTATION_START
	public:
		void render(PxeRenderTarget& renderTarget);
		void removeShader(PxeShader& shader);
		void removeFontFile(PxeFontFile& file);
		void updateFontAtlas();
		PXE_NODISCARD PxeGlImGuiBackend* getGuiBackend();
		PXE_NODISCARD ImFontAtlas* getFontAtlas();

	private:
		friend class PxeEngine;
		struct Impl;

		inline Impl& impl() { return reinterpret_cast<Impl&>(_storage); }
		inline const Impl& impl() const { return reinterpret_cast<const Impl&>(_storage); }
		void initializePipeline();
		void uninitializePipeline();
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_RENDER_PIPELINE_H_
