#include "DuckGfx_internal.h"

namespace duckGfx {
  void Camera::RefreshProjMatrix() {
    m_proj = Matrix4::BuildPerspProj(m_wFov, m_AspectRatio, m_near, m_far) * Matrix4::BuildScaleMatrix(Vec3(1, 1, -1));
  }

  void Camera::RefreshProjView() {
    m_viewProj = m_proj * m_transform.CalcInvMatrix();
  }
}