#pragma once
#include "dmath.h"
#include "stringhash.h"
#include "windows.h"

namespace duckGfx {
  bool Init(HWND window, uint32_t width, uint32_t height);
  void Render();
  void Shutdown();

  class IMaterial {
  public:
    static void Destroy(IMaterial * mat);
  };

  class IMaterialInstance {
  public:
    static IMaterialInstance * Create(IMaterial * mat);
    static void Destroy(IMaterialInstance * inst);

    virtual bool SetParameter(StringHash name, float * data, uint32_t count) = 0;
    virtual bool SetParameter(StringHash name, uint32_t * data, uint32_t count) = 0;
    virtual bool SetParameter(StringHash name, int32_t * data, uint32_t count) = 0;

    virtual bool SetParameter(StringHash name, float data) = 0;
    virtual bool SetParameter(StringHash name, uint32_t data) = 0;
    virtual bool SetParameter(StringHash name, int32_t data) = 0;

    virtual bool SetParameter(StringHash name, const Vec2 & data) = 0;
    virtual bool SetParameter(StringHash name, const Vec3 & data) = 0;
    virtual bool SetParameter(StringHash name, const Vec4 & data) = 0;

    virtual bool SetParameter(StringHash name, const Matrix4 & data) = 0;
  };
  

  class IModel {
  public:
    static void Destroy(IModel * arg);

    virtual void SetTransform(const TransformSRT & arg) = 0;
    virtual void SetMaterialInstance(IMaterialInstance * instance) = 0;
  };  

  class ICamera {
  public:
    static ICamera * Create();
    static void Destroy(ICamera * arg);

    virtual void SetTransform(const TransformRT & arg) = 0;
    virtual void SetPerspective(float wFov, float aspectRatio, float nearPlane, float farPlane) = 0;
  };

  class IScene {
  public:
    virtual void AddModel(IModel * model) = 0;
    virtual void RemoveModel(IModel * model) = 0;
  
    virtual void SetMainCamera(ICamera * camera) = 0;
  };
  IScene * GetScene();

}