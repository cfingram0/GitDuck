#pragma once
#include "sample_framework.h"

class Lighting_Test : public ISample {
  void Init() override;
  void OnStart() override;
  void Update(float dt) override;
  void OnEnd() override;
  void Shutdown() override;

  duckGfx::IScene * m_scene = nullptr;
  duckGfx::IModel * m_triangle = nullptr;
  duckGfx::IMaterial * m_material = nullptr;
  duckGfx::IMaterial * m_lightMaterial = nullptr;
  duckGfx::IMaterialInstance * m_matInst = nullptr;

  duckGfx::ICamera * m_camera = nullptr;
  TransformRT m_cameraTrans{ tag::NoInit{} };

  duckGfx::IModel * m_model2 = nullptr;
  duckGfx::IMaterialInstance * m_matInst2 = nullptr;
  TransformSRT m_modelTrans2{ tag::NoInit{} };

  duckGfx::IPointLight * m_pLight = nullptr;
  duckGfx::IModel * m_pLightModel = nullptr;
  duckGfx::IMaterialInstance * m_pLightMat = nullptr;

  duckGfx::ISpotLight * m_sLight = nullptr;
  duckGfx::IModel * m_sLightModel = nullptr;
  duckGfx::IMaterialInstance * m_sLightMat = nullptr;
  TransformSRT m_sLightTrans{ tag::NoInit{} };

  duckGfx::ISpotLight * m_sLight2 = nullptr;
  duckGfx::IModel * m_sLight2Model = nullptr;
  duckGfx::IMaterialInstance * m_sLight2Mat = nullptr;
  TransformSRT m_sLight2Trans{ tag::NoInit{} };

  duckGfx::IPointLight * m_pLight2 = nullptr;
  duckGfx::IModel * m_pLight2Model = nullptr;
  duckGfx::IMaterialInstance * m_pLight2Mat = nullptr;

  float m_angle = 0;
  TransformSRT m_modelTrans{ tag::NoInit{} };
};