#include <stdint.h>

#include "PxeRefCount.h"

#ifndef PXENGINE_RENDER_BASE_H_
#define PXENGINE_RENDER_BASE_H_
namespace pxengine {
	class PxeScene;

	class PxeRenderBase : public PxeRefCount
	{
	public:
		enum class RenderSpace : uint8_t
		{
			NONE = 0,
			SCREEN_SPACE,
			WORLD_SPACE,
		};

		inline PxeRenderBase(RenderSpace renderSpace) : _renderSpace(renderSpace) {}
		inline virtual ~PxeRenderBase() = default;

		RenderSpace getRenderSpace() const { return _renderSpace; }

		virtual void render() = 0;

		PxeRenderBase(const PxeRenderBase& other) = delete;
		PxeRenderBase operator=(const PxeRenderBase& other) = delete;

	private:
		friend class PxeScene;
		const RenderSpace _renderSpace;
	};
}
#endif // !PXENGINE_RENDER_BASE_H_