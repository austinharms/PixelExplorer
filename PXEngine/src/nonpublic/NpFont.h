#ifndef PXENGINE_NONPUBLIC_FONT_H_
#define PXENGINE_NONPUBLIC_FONT_H_
#include "PxeTypes.h"
#include "PxeFont.h"
#include "NpFontSource.h"

namespace pxengine {
	namespace nonpublic {
		class NpFont : public PxeFont
		{
		public:
			NpFont(NpFontSource& source, uint16_t size, PxeFontScalingMode mode, const glm::i32vec2& windowRef);
			virtual ~NpFont();
			void guiPushFont() override;
			void guiPopFont() override;
			PXE_NODISCARD uint16_t getPixelSize() const override;
			PXE_NODISCARD uint16_t getScaledPixelSize() const override;
			PXE_NODISCARD const std::filesystem::path& getFontSource() const override;
			PXE_NODISCARD PxeFontScalingMode getScalingMode() const override;
			PXE_NODISCARD const glm::i32vec2& getReferenceWindowSize() const override;
			PXE_NODISCARD PxeFontStatus getStatus() const override;

		private:
			NpFontSource* _source;
			const uint16_t _size;
			const glm::i32vec2 _windowRefSize;
			const PxeFontScalingMode _scaleMode;
			float _oldFontScale;
		};
	}
}
#endif // !PXENGINE_NONPUBLIC_FONT_H_
