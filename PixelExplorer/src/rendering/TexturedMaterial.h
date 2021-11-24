#ifndef RENDER_TEXTUREDMATERIAL_H
#define RENDER_TEXTUREDMATERIAL_H

#include "Material.h"

class TexturedMaterial : public Material
{
public:
	TexturedMaterial(Shader* shader, void* _texture, int32_t width, int32_t height);
	virtual ~TexturedMaterial();
	void onPostBind() override;

	int32_t getWidth() const { return _width; }

	int32_t getHeight() const { return _height; }

	void updateTexture(void* _texture, int32_t width, int32_t height);

private:
	void bindTexture();
	void unbindTexture();

	int32_t _width;
	int32_t _height;
	uint32_t _textureId;
};

#endif