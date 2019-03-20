#include "DuckGfx_internal.h"

namespace duckGfx {
  IPointLight * IPointLight::Create() {
    return new PointLight();
  }

  void IPointLight::Destroy(IPointLight * arg) {
    delete reinterpret_cast<PointLight *>(arg);
  }

  void PointLight::SetPosition(const Vec3 & pos) {
    m_position = pos;
  }

  void PointLight::SetColor(const Vec3 & color) {
    m_color = color;
  }

  void PointLight::SetIntensity(float arg) {
    m_intensity = arg;
  }

  ISpotLight * ISpotLight::Create() {
    return new SpotLight();
  }

  void ISpotLight::Destroy(ISpotLight * arg) {
    delete reinterpret_cast<SpotLight*>(arg);
  }

  void SpotLight::SetDirection(const Vec3 & dir) {
    m_direction = Normalize(dir);
  }

  void SpotLight::SetPosition(const Vec3 & pos) {
    m_position = pos;
  }

  void SpotLight::SetColor(const Vec3 & color) {
    m_color = color;
  }

  void SpotLight::SetIntensity(float arg) {
    m_intensity = arg;
  }

  void SpotLight::SetAngle(float innerAngle, float outAngle) {
    m_cosInnerAngle = std::cos((3.14f *innerAngle) / 180.0f);
    m_cosOuterAngle = std::cos((3.14f *outAngle) / 180.0f);
  }
}