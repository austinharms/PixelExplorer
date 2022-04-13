#ifndef RENDER_TEXTUREDMATERIAL_H
#define RENDER_TEXTUREDMATERIAL_H
#include <stdint.h>

#include "../Material.h"
#include "../Shader.h"
namespace px::rendering {
class TexturedMaterial : public Material {
 public:
  TexturedMaterial(Shader* shader, void* _texture, int32_t width,
                   int32_t height);
  virtual ~TexturedMaterial();
  void bind() override;
  int32_t getWidth() const { return _width; }
  int32_t getHeight() const { return _height; }
  void updateTexture(void* _texture, int32_t width, int32_t height);
  Shader* getShader() const;

 private:
  void bindTexture();
  void unbindTexture();

  Shader* _shader;
  int32_t _width;
  int32_t _height;
  uint32_t _textureId;
};
}  // namespace px::rendering
#endif