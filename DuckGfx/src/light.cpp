#include "DuckGfx_internal.h"

namespace duckGfx {
  IPointLight * IPointLight::Create() {
    return new PointLight();
  }
  void IPointLight::Destroy(IPointLight * arg) {
    delete arg;
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
}