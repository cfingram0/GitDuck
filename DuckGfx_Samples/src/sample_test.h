#pragma once
#include "sample_framework.h"

class Sample_Test : public ISample {
  void Init() override;
  void OnStart() override;
  void Update(float dt) override;
  void OnEnd() override;
  void Shutdown() override;

  duckGfx::IScene * m_scene = nullptr;
  duckGfx::IModel * m_triangle = nullptr;
  duckGfx::IMaterial * m_material = nullptr;
  duckGfx::IMaterialInstance * m_matInst = nullptr;
  float m_angle = 0;
  TransformSRT m_modelTrans{ tag::NoInit{} };
};