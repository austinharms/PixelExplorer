#ifndef PXENGINE_SHADER_H_
#define PXENGINE_SHADER_H_
PXE_PRIVATE_IMPLEMENTATION_START
#include <mutex>
PXE_PRIVATE_IMPLEMENTATION_END

#include "PxeTypes.h"
#include "PxeGLAsset.h"
#include "PxeGlBindable.h"
#include "PxeRenderTarget.h"

namespace pxengine {
	class PxeRenderProperties;
	class PxeRenderPipeline;

	// Class for setting up OpenGl state when rendering objects
	class PxeShader : public PxeGLAsset, public PxeGlBindable
	{
	public:
		// TODO Comment functions
		PXE_NODISCARD virtual PxeShaderId getShaderId() = 0;
		PXE_NODISCARD virtual PxeShaderExecutionOrder getExecutionOrder() const = 0;

		template<class T, typename std::enable_if<std::is_base_of<PxeShader, T>::value>::type* = nullptr>
		PXE_NODISCARD inline static PxeShaderId getShaderId() {
			static PxeShaderId shaderId = 0;
			if (shaderId == 0) shaderId = getNextShaderId();
			return shaderId;
		}

		template<>
		PXE_NODISCARD inline static uint32_t getShaderId<PxeShader>() { return 0; }

		virtual ~PxeShader();
		PXE_NOCOPY(PxeShader);

	protected:
		PxeShader(bool delayAssetInitialization = false);
		// TODO Comment functions
		virtual void initializeGl() override = 0;
		virtual void uninitializeGl() override = 0;
		virtual void bind() override = 0;
		virtual void setRenderTarget(PxeRenderTarget& renderTarget) {}
		virtual void unbind() override = 0;
		virtual void updateProperties(const PxeRenderProperties& renderProperties) = 0;

	private:
		static PxeShaderId getNextShaderId();
		PXE_PRIVATE_IMPLEMENTATION_START
	private:
		friend class PxeRenderPipeline;
		static std::mutex s_shaderIdMutex;
		static uint32_t s_shaderIdCounter;
		PXE_PRIVATE_IMPLEMENTATION_END
	};
}
#endif // !PXENGINE_SHADER_H_
