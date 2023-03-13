#ifndef PXENGINE_GUI_SHADER_H_
#define PXENGINE_GUI_SHADER_H_
#include "PxeTypes.h"
#include "PxeGlShaderProgram.h"
#include "PxeShader.h"

namespace pxengine {
	class PxeGuiShader : public PxeShader
	{
	public:
		PxeGuiShader();
		virtual ~PxeGuiShader();
		PXE_NODISCARD PxeShaderId getShaderId() override;
		PXE_NODISCARD PxeShaderExecutionOrder getExecutionOrder() const override;
		void setProjectionMatrix(const glm::mat4& mat);
		void setTexture(uint32_t texture);

	protected:
		void initializeGl() override;
		void uninitializeGl() override;
		void bind() override;
		void unbind() override;

	private:
		PxeGlShaderProgram* _glProgram;
		int32_t _textureLocation;
		int32_t _projectionLocation;
	};
}
#endif // !PXENGINE_GUI_SHADER_H_
