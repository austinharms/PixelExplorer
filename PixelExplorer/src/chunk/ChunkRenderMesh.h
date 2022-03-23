#ifndef CHUNKRENDERMESH_H
#define CHUNKRENDERMESH_H

#include <mutex>

#include "DataBuffer.h"
#include "rendering/Material.h"
#include "rendering/Renderable.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"


class ChunkRenderMesh : public Renderable {
 public:
  static void SetMaterial(Material* mat);
  static void DropMaterial();

  ChunkRenderMesh();
  ~ChunkRenderMesh();
  bool ShouldDrop() const;
  Material* GetMaterial() const;
  bool PreRender(float deltaTime, const glm::vec3& cameraPos,
                 const glm::vec3& cameraRotation);
  glm::mat4 GetTransform() const;
  void Render() const;
  void SetPosition(glm::vec3 pos);
  void SetDropFlag();
  void UpdateBuffers(DataBuffer<float>* verts, DataBuffer<uint32_t>* indices);
  bool GetError() const;
  void SetActive(bool active);

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

  void UpdateBuffers();
};
#endif  // !CHUNKRENDERMESH_H