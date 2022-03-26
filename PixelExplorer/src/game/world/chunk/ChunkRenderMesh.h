#ifndef CHUNKRENDERMESH_H
#define CHUNKRENDERMESH_H

#include <mutex>

#include "DataBuffer.h"
#include "rendering/Material.h"
#include "rendering/Renderable.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace px::game::chunk {
class ChunkRenderMesh : public Renderable {
 public:
  static void setMaterial(Material* mat);
  static void dropMaterial();

  ChunkRenderMesh();
  ~ChunkRenderMesh();
  bool shouldDrop() const;
  Material* getMaterial() const;
  bool preRender(float deltaTime, const glm::vec3& cameraPos,
                 const glm::vec3& cameraRotation);
  glm::mat4 getTransform() const;
  void render() const;
  void setPosition(glm::vec3 pos);
  void setDropFlag();
  void updateBuffers(DataBuffer<float>* verts, DataBuffer<uint32_t>* indices);
  bool getError() const;
  void setActive(bool active);

 private:
  static Material* s_material;

  DataBuffer<float>* _vertexBuffer;
  DataBuffer<uint32_t>* _indexBuffer;
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