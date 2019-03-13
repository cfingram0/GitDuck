#pragma once
#include <d3d11.h>
#include <d3d11_1.h>
#include <cstdio>
#include <cassert>
#include <cstdint>
#include <vector>

#include "debug.h"
#include "dmath.h"
#include "io.h"
#include "stringhash.h"

namespace duckGfx {
  class Camera {
  public:
    TransformRT m_transform{ tag::Identity{} };
    Matrix4 m_proj{ tag::NoInit{} };
    Matrix4 m_viewProj{ tag::NoInit{} };
    float m_wFov;
    float m_AspectRatio;
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

    // depth target
    ID3D11Texture2D * m_depthStencil = nullptr;
    ID3D11DepthStencilView * m_depthStencilView = nullptr;
  };

  void BindRenderTarget2D(ID3D11DeviceContext * context, const RenderTarget2D & rt);


  struct VertexFormat {
    VertexFormat (): combinedVF(0) {}
    VertexFormat(const VertexFormat & rhs) : combinedVF(rhs.combinedVF) {}

    VertexFormat & operator= (const VertexFormat & rhs) {
      combinedVF = rhs.combinedVF;
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


  class Model {
  public:
    ~Model() {
      if (m_theMesh) {
        delete m_theMesh;
        m_theMesh = nullptr;
      }
    }
    // only one mesh per model now, will change later
    Mesh * m_theMesh = nullptr;
    TransformSRT m_transform{ tag::Identity{} };
  };


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

  class Material {
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

  bool CreateTestMaterial(ID3D11Device * device, Material * outMaterial);


  class MaterialInstance {
  public:
    MaterialInstance(Material * material);
    bool SetParameter(StringHash name, float * data, uint32_t count);
    bool SetParameter(StringHash name, uint32_t * data, uint32_t count);
    bool SetParameter(StringHash name, int32_t * data, uint32_t count);
    
    bool SetParameter(StringHash name, float data);
    bool SetParameter(StringHash name, uint32_t data);
    bool SetParameter(StringHash name, int32_t data);

    bool SetParameter(StringHash name, const Vec2 & data);
    bool SetParameter(StringHash name, const Vec3 & data);
    bool SetParameter(StringHash name, const Vec4 & data);

    bool SetParameter(StringHash name, const Matrix4 & data);

    void WriteToBuffers(const Material::VariableMap * varInfo, void * data, uint32_t size);

    Material * m_material = nullptr;
    std::vector<uint8_t> m_cpuVsConstantBuffer[MaterialTechniqueID::kCount];
    std::vector<uint8_t> m_cpuPsConstantBuffer[MaterialTechniqueID::kCount];
  };

  void MapMaterialInstanceData(ID3D11DeviceContext * context, Material * mat, MaterialInstance * inst, MaterialTechniqueID technique);

  bool CreateVertLayout(ID3D11Device * device, const VertexFormat & fmt, void * shaderByteCode, size_t shaderbyteCodeLength, ID3D11InputLayout ** outLayout);


  struct DuckContext {
    IDXGISwapChain * pSwapChain = nullptr;
    ID3D11Device * pDevice = nullptr;
    ID3D11DeviceContext * pImmediateContext = nullptr;

    // render target
    RenderTarget2D * pBackBufferRt = nullptr;

    // To backbuffer Render state
    ID3D11RasterizerState * toBackbufferRS = nullptr;
    ID3D11DepthStencilState * toBackbufferDSS = nullptr;
    ID3D11Buffer * toBackBufferPassCb = nullptr;

    Camera * camera = nullptr;

    Model * testTriangle = nullptr;

    ID3DUserDefinedAnnotation * annotator = nullptr;

    // material data
    Material * material = nullptr;
    MaterialInstance * matInst = nullptr;
  };

  extern DuckContext globalContext;

}