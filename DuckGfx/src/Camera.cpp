#include "DuckGfx_internal.h"

namespace duckGfx {
  ICamera * ICamera::Create() {
    return new Camera();
  }

  void ICamera::Destroy(ICamera * arg) {
    delete reinterpret_cast<Camera*>(arg);
  }

  void Camera::SetTransform(const TransformRT & arg) {
    m_transform = arg;
    RefreshProjView();
  }

  void Camera::SetPerspective(float wFov, float aspectRatio, float nearPlane, float farPlane) {
    m_near = nearPlane;
    m_far = farPlane;
    m_wFov = wFov;
    m_aspectRatio = aspectRatio;

    RefreshProjMatrix();
    RefreshProjView();
  }

  Vec3 Camera::GetLookAt() {
    return m_transform.rotation.RotateVec(Vec4(0, 0, -1, 0)).xyz();
  }

  void Camera::RefreshProjMatrix() {
    m_proj = Matrix4::BuildPerspProj(m_wFov, m_aspectRatio, m_near, m_far) * Matrix4::BuildScaleMatrix(Vec3(1, 1, -1));
  }

  void Camera::RefreshProjView() {
    m_viewProj = m_proj * m_transform.CalcInvMatrix();
  }
}