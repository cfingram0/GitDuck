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
  
  struct VertexFormat {
    VertexFormat() : combinedVF(0) {}
    VertexFormat(const VertexFormat & rhs) : combinedVF(rhs.combinedVF) {}

    VertexFormat & operator= (const VertexFormat & rhs) {
      combinedVF = rhs.combinedVF;
      return *this;
    }

    bool operator==(const VertexFormat & rhs) {
      return combinedVF == rhs.combinedVF;
    }

    union {
      struct {
        uint8_t hasPosition : 1;
        uint8_t hasNormals : 1;
        uint8_t hasTangentFrame : 1; // tangent + bitangent
        uint8_t hasBlendWeightsAndIndices : 1;
        uint8_t numUVsets : 4;
        uint8_t numColorSets : 4;
      };

      uint32_t combinedVF;
    };
  };

  uint32_t VertSize(const VertexFormat & fmt);

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

  class IPointLight {
  public:
    static IPointLight * Create();
    static void Destroy(IPointLight * arg);

    virtual void SetPosition(const Vec3 & pos) = 0;
    virtual void SetColor(const Vec3 & color) = 0;
    virtual void SetIntensity(float arg) = 0;
  };

  class ISpotLight{
  public:
    static ISpotLight * Create();
    static void Destroy(ISpotLight * arg);

    virtual void SetDirection(const Vec3 & dir) = 0;
    virtual void SetPosition(const Vec3 & pos) = 0;
    virtual void SetColor(const Vec3 & color) = 0;
    virtual void SetIntensity(float arg) = 0;
    virtual void SetAngle(float innerAngleDeg, float outerAngleDeg) = 0;
  };

  class IScene {
  public:
    virtual void AddModel(IModel * model) = 0;
    virtual void RemoveModel(IModel * model) = 0;
  
    virtual void SetMainCamera(ICamera * camera) = 0;

    // main light
    virtual void SetLightDir(const Vec3 & rhs) = 0;
    virtual void SetlightColor(const Vec3 & rhs) = 0;
    virtual void SetAmbientColor(const Vec3 & rhs) = 0;

    virtual void AddLight(IPointLight * light) = 0;
    virtual void RemoveLight(IPointLight * light) = 0;

    virtual void AddLight(ISpotLight * light) = 0;
    virtual void RemoveLight(ISpotLight * light) = 0;

    virtual void DrawDebugLine(const Vec3 & a, const Vec3 & b, const Vec3 & color) = 0;
    virtual void DrawDebugSphere(const Vec3 & pos, float r, const Vec3 & color) = 0;
    virtual void DrawDebugAABB(const Vec3 & min, const Vec3 & max, const Vec3 & color) = 0;
    virtual void DrawDebugOBB(TransformSRT & transform, const Vec3 & color) = 0; // by default cube is 1,1,1 at origin
    virtual void DrawDebugCone(const Vec3 & tip, float halfAngle, float length, const Vec3 & direction, const Vec3 & color) = 0;
  };
  IScene * GetScene();



}