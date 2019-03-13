#pragma once
#include "sample_framework.h"

class Sample_Test : public ISample {
  void Init() override;
  void OnStart() override;
  void Update(float dt) override;
  void OnEnd() override;
  void Shutdown() override;
};