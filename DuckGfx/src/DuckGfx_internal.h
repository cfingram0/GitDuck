#pragma once
#include <d3d11.h>
#include <d3d11_1.h>
#include <cstdio>
#include <cassert>
#include <cstdint>
#include <vector>
#include <cmath>

#include "DuckGfx.h"

#include "debug.h"
#include "dmath.h"
#include "io.h"
#include "stringhash.h"

namespace duckGfx {
  class Camera : public ICamera {
  public:
    void SetTransform(const TransformRT & arg) override;
    void SetPerspective(float wFov, float aspectRatio, float nearPlane, float farPlane) override;

    Vec3 GetLookAt();

    TransformRT m_transform{ tag::Identity{} };
    Matrix4 m_proj{ tag::NoInit{} };
    Matrix4 m_viewProj{ tag::NoInit{} };
    float m_wFov;
    float m_aspectRatio;
    float m_near;
    float m_far;

    void RefreshProjMatrix();
    void RefreshProjView();
  };


  struct RenderTarget2D {
    ~RenderTarget2D();
    // color targets
    uint8_t m_numColorTargets = 0;
    ID3D11Texture2D * m_colorTargets[8] = { 0 };
    ID3D11RenderTargetView * m_colorViews[8] = { 0 };
    ID3D11ShaderResourceView * m_colorSRV[8] = { 0 };

    // depth target
    ID3D11Texture2D * m_depthStencil = nullptr;
    ID3D11DepthStencilView * m_depthStencilView = nullptr;
    ID3D11ShaderResourceView * m_depthStencilSRV = nullptr;
  };

  void BindRenderTarget2D(ID3D11DeviceContext * context, const RenderTarget2D & rt);

  enum MaterialTechniqueID {
    kColor,
    kDepthOnly,
    kCount
  };

  // don't support bool since varying bool sizes are likely 
  // to cause headaches
  enum MaterialParameterType {
    kUint,   // 32-bit
    kInt,    // 32-bit
    kFloat,  // 32-bit
  };

  // start with just one constant buffer, with all of the per-object uniforms
  class MaterialTechnique {
  public:
    ID3D11VertexShader * m_vertShader = nullptr;
    ID3D11Buffer * m_vsConstantBuffer = nullptr;
    uint32_t m_vsConstantBufferSize = 0;
    uint32_t m_vsConstantBufferSlot = 0;

    ID3D11PixelShader * m_pixelShader = nullptr;
    ID3D11Buffer * m_psConstantBuffer = nullptr;
    uint32_t m_psConstantBufferSize = 0;
    uint32_t m_psConstantBufferSlot = 0;
  };

  class Material  : public IMaterial {
  public:
    ~Material();
    struct VariableMap {
      VariableMap() {
        for (int32_t i = 0; i < MaterialTechniqueID::kCount; ++i) {
          dataStartPs[i] = -1;
          dataStartVs[i] = -1;
        }
        elementType = MaterialParameterType::kFloat;
        numValues = 0;
      }

      StringHash varName;
      MaterialParameterType elementType;
      uint8_t numValues;

      // starting point in index for each shader, -1 means the 
      // variable isn't in that particular buffer
      int32_t dataStartPs[MaterialTechniqueID::kCount];
      int32_t dataStartVs[MaterialTechniqueID::kCount];
    };

    std::vector<VariableMap> variables;
    MaterialTechnique techniques[MaterialTechniqueID::kCount];
    ID3D11InputLayout * inputLayout = nullptr;
    VertexFormat meshVertFormat;

    const VariableMap * FindVariableData(StringHash name) const;
  };

  void BindMaterial(ID3D11DeviceContext * context, Material * mat, MaterialTechniqueID technique);


  class MaterialInstance : public IMaterialInstance {
  public:

    // local
    MaterialInstance(Material * material);
    void WriteToBuffers(const Material::VariableMap * varInfo, void * data, uint32_t size);

    // IMaterialInstance
    bool SetParameter(StringHash name, float * data, uint32_t count) override;
    bool SetParameter(StringHash name, uint32_t * data, uint32_t count) override;
    bool SetParameter(StringHash name, int32_t * data, uint32_t count) override;
    
    bool SetParameter(StringHash name, float data) override;
    bool SetParameter(StringHash name, uint32_t data) override;
    bool SetParameter(StringHash name, int32_t data) override;

    bool SetParameter(StringHash name, const Vec2 & data) override;
    bool SetParameter(StringHash name, const Vec3 & data) override;
    bool SetParameter(StringHash name, const Vec4 & data) override;

    bool SetParameter(StringHash name, const Matrix4 & data) override;

    Material * m_material = nullptr;
    std::vector<uint8_t> m_cpuVsConstantBuffer[MaterialTechniqueID::kCount];
    std::vector<uint8_t> m_cpuPsConstantBuffer[MaterialTechniqueID::kCount];
  };

  void MapMaterialInstanceData(ID3D11DeviceContext * context, Material * mat, MaterialInstance * inst, MaterialTechniqueID technique);

  bool CreateVertLayout(ID3D11Device * device, const VertexFormat & fmt, void * shaderByteCode, size_t shaderbyteCodeLength, ID3D11InputLayout ** outLayout);

  struct Mesh {
    ~Mesh() {
      vertBuffer->Release();
      vertBuffer = nullptr;
      idxBuffer->Release();
      idxBuffer = nullptr;
    }

    StringHash matId;
    VertexFormat format;

    uint32_t vertSize = 0;
    ID3D11Buffer * vertBuffer = nullptr;

    uint32_t idxCount = 0;
    ID3D11Buffer * idxBuffer = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
  };

  void BindMesh(ID3D11DeviceContext * context, const Mesh & mesh);


  class Model : public IModel {
  public:
    ~Model() {
      if (m_theMesh) {
        delete m_theMesh;
        m_theMesh = nullptr;
      }
    }

    void SetTransform(const TransformSRT & arg) override;
    void SetMaterialInstance(IMaterialInstance * instance) override;

    // only one mesh per model now, will change later
    Mesh * m_theMesh = nullptr;
    MaterialInstance * m_matInstance = nullptr;
    TransformSRT m_transform{ tag::Identity{} };
  };


  class PointLight : public IPointLight {
  public:
    void SetPosition(const Vec3 & pos) override;
    void SetColor(const Vec3 & color) override;
    void SetIntensity(float arg) override;

    float m_intensity = 1;
    Vec3 m_position = tag::Zero{};
    Vec3 m_color = {1, 1, 1};
  };

  class SpotLight : public ISpotLight {
  public:
    SpotLight() {
      m_cosInnerAngle = std::cos((60.0f * 3.14f) / 180.0f);
      m_cosOuterAngle = std::cos((60.0f * 3.14f) / 180.0f);
    }

    void SetDirection(const Vec3 & dir) override;
    void SetPosition(const Vec3 & pos) override;
    void SetColor(const Vec3 & color) override;
    void SetIntensity(float arg) override;
    void SetAngle(float innerAngleDeg, float outerAngleDeg) override;

    Vec3 m_position = tag::Zero{};
    Vec3 m_direction = { 0.0f, -1.0f, 0.0f };
    Vec3 m_color = {1, 1, 1};

    float m_intensity = 1;
    float m_cosInnerAngle;
    float m_cosOuterAngle;;
  };

  // debug primitives
  struct DebugLine {
    Vec3 p0;
    Vec3 p1;
    Vec3 color;
  };
  bool GenerateDebugLineMesh(Mesh * output);

  //utils
  bool GenerateQuad(ID3D11Device * device, Mesh * output);
  bool CreateToBackbufferMaterial(ID3D11Device * device, Material * outMaterial);

  class Scene : public IScene {
  public:
    std::vector<Model*> m_models;
    Camera * m_mainCamera;

    Vec3 m_lightDirection = {0, 1, 0};
    Vec3 m_lightColor = { 1,1,1 };
    Vec3 m_ambientColor = { 0,0,0 };

    std::vector<PointLight*> m_pointLights;
    std::vector<SpotLight*> m_spotLights;
    std::vector<DebugLine> m_debugLines;

    void AddModel(IModel * model) override;
    void RemoveModel(IModel * model) override;
    
    void SetMainCamera(ICamera * camera) override;
    
    void SetLightDir(const Vec3 & rhs) override;
    void SetlightColor(const Vec3 & rhs) override;
    void SetAmbientColor(const Vec3 & rhs) override;

    void AddLight(IPointLight * light) override;
    void RemoveLight(IPointLight * light) override;

    void AddLight(ISpotLight * light) override;
    void RemoveLight(ISpotLight * light) override;

    void DrawDebugLine(const Vec3 & a, const Vec3 & b, const Vec3 & color) override;
    void DrawDebugSphere(const Vec3 & pos, float r, const Vec3 & color) override;
    void DrawDebugAABB(const Vec3 & min, const Vec3 & max, const Vec3 & color) override;
    void DrawDebugOBB(TransformSRT & transform, const Vec3 & color) override;
    void DrawDebugCone(const Vec3 & tip, float halfAngle, float length, const Vec3 & direction, const Vec3 & color) override;
  };

  struct DuckContext {
    uint32_t width = 0;
    uint32_t height = 0;

    IDXGISwapChain * pSwapChain = nullptr;
    ID3D11Device * pDevice = nullptr;
    ID3D11DeviceContext * pImmediateContext = nullptr;

    // global lighting cb data
    struct LightingData {
      LightingData() {
      }
      union {
        struct {
          Vec4 cameraPos;
          Vec4 lightDir;
          Vec4 lightColor;
          Vec4 ambient;

          uint32_t numLights[4];

          Vec4 pointPos[8];
          Vec4 pointColor[8];

          struct {
            Vec3 spotPos;
            float spotCosInnerAngle;
          } spotPos[8];

          struct {
            Vec3 spotDirection;
            float spotCosOuterAngle;
          } spotDir[8];

          struct {
            Vec3 spotColor;
            float spotIntensity;
          } spotInten[8];
        };
        float data[180] = {0};
      };
    } lightingDataBuffer;
    ID3D11Buffer * lightingDataCb = nullptr;

    // main color rt
    RenderTarget2D * pMainColorRt = nullptr;
    ID3D11RasterizerState * toMainColorRS = nullptr;
    ID3D11DepthStencilState * toMainColorDSS = nullptr;
    ID3D11Buffer * toMainColorCb = nullptr;

    // To backbuffer Render state
    RenderTarget2D * pBackBufferRt = nullptr;
    ID3D11RasterizerState * toBackbufferRS = nullptr;
    ID3D11DepthStencilState * toBackbufferDSS = nullptr;
    ID3D11Buffer * toBackBufferPassCb = nullptr;

    Mesh * backBufferQuad = nullptr;
    Material * toBackBufferMat = nullptr;
    MaterialInstance * toBackBufferMatInstance = nullptr;
    ID3D11SamplerState * toBackBufferSamplerSate = nullptr;


    // debug data
    ID3DUserDefinedAnnotation * annotator = nullptr;

    //debug draw data
    Mesh * debugLineMesh = nullptr;
    Material * debugDrawMaterial = nullptr;
    MaterialInstance * debugMaterialInstance = nullptr;

    Scene theScene;
  };

  extern DuckContext globalContext;

}