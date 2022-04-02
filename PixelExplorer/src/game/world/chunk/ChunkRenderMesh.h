#ifndef CHUNKRENDERMESH_H
#define CHUNKRENDERMESH_H

#include <mutex>

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "rendering/Material.h"
#include "rendering/Renderable.h"
#include "util/DataBuffer.h"
namespace px::game::chunk {
class ChunkRenderMesh : public rendering::Renderable {
 public:
  ChunkRenderMesh(rendering::Material* material);
  ~ChunkRenderMesh();
  bool shouldDrop() const;
  rendering::Material* getMaterial() const;
  bool preRender(float deltaTime, const glm::vec3& cameraPos,
                 const glm::vec3& cameraRotation);
  glm::mat4 getTransform() const;
  void render() const;
  void setPosition(glm::vec3 pos);
  void setDropFlag();
  void updateBuffers(util::DataBuffer<float>* verts,
                     util::DataBuffer<uint32_t>* indices);
  bool getError() const;
  void setActive(bool active);

 private:
  rendering::Material* _material;
  util::DataBuffer<float>* _vertexBuffer;
  util::DataBuffer<uint32_t>* _indexBuffer;
  uint32_t _indexCount;
  uint32_t _indexBufferId;
  uint32_t _vertexArrayId;
  uint32_t _vertexBufferId;
  glm::mat4 _transform;
  std::mutex _bufferMutex;
  bool _active;
  bool _drop;
  bool _dirty;
  bool _error;

  void updateBuffers();
};
}  // namespace px::game::chunk
#endif  // !CHUNKRENDERMESH_H