#pragma once
#include "sample_framework.h"

class ModelLoad_Test : public ISample {
  void Init() override;
  void OnStart() override;
  void Update(float dt) override;
  void OnEnd() override;
  void Shutdown() override;

  duckGfx::IScene * m_scene = nullptr;
  duckGfx::IMaterial * m_material = nullptr;

  duckGfx::ICamera * m_camera = nullptr;
  TransformRT m_cameraTrans{ tag::NoInit{} };

  duckGfx::IModel * m_model2 = nullptr;
  duckGfx::IMaterialInstance * m_matInst2 = nullptr;
  TransformSRT m_modelTrans2{ tag::NoInit{} };


  duckGfx::IModel * m_model3 = nullptr;
  duckGfx::IMaterialInstance * m_matInst3 = nullptr;
  TransformSRT m_modelTrans3{ tag::NoInit{} };


  float m_time = 0.0f;
};