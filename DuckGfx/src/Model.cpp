#include "DuckGfx_internal.h"

namespace duckGfx {
  void IModel::Destroy(IModel * arg) {
    Model * intern = reinterpret_cast<Model*>(arg);
    delete intern;
  }

  uint32_t VertSize(const VertexFormat & fmt) {
    uint32_t sizeAccum = 0;
    if (fmt.hasPosition) {
      sizeAccum += sizeof(Vec3);
    }
    if (fmt.hasNormals) {
      sizeAccum += sizeof(Vec3);
    }
    if (fmt.hasTangentFrame) {
      sizeAccum += 2 * sizeof(Vec3);
    }
    if (fmt.hasBlendWeightsAndIndices) {
      sizeAccum += 4 * (sizeof(float) + sizeof(uint32_t));
    }
    if (fmt.numUVsets) {
      sizeAccum += fmt.numUVsets * sizeof(Vec2);
    }
    if (fmt.numColorSets) {
      sizeAccum += fmt.numColorSets * sizeof(Vec4);
    }
    return sizeAccum;
  }

  void Model::SetTransform(const TransformSRT & arg) {
    m_transform = arg;
  }

  void Model::SetMaterialInstance(IMaterialInstance * instance) {
    m_matInstance = reinterpret_cast<MaterialInstance *>(instance);
  }

  void BindMesh(ID3D11DeviceContext * context, const Mesh & mesh) {
    // hey look, the place where real rendering happens
    uint32_t stride = mesh.vertSize;
    uint32_t offset = 0;

    // mesh data
    context->IASetVertexBuffers(0, 1, &mesh.vertBuffer, &stride, &offset);
    context->IASetIndexBuffer(mesh.idxBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(mesh.topology);
  }
}