#pragma once

class ISample {
public:
  virtual void Init() = 0;
  virtual void OnStart() = 0;
  virtual void Update(float dt) = 0;
  virtual void OnEnd() = 0;
  virtual void Shutdown() = 0;
};