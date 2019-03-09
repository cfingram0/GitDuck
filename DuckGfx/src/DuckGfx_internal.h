#pragma once
#include <d3d11.h>
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
    ID3D11Texture2D * m_colorTargets[8] = { 0 };
    uint8_t m_numColorTargets = 0;
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

  struct DuckContext {
    IDXGISwapChain * pSwapChain = nullptr;
    ID3D11Device * pDevice = nullptr;
    ID3D11DeviceContext * pImmediateContext = nullptr;

    // render target
    RenderTarget2D * pBackBufferRt = nullptr;

    // pass data
    ID3D11RasterizerState * rasterizorState = nullptr;
    ID3D11DepthStencilState * depthStencilState = nullptr;

    Camera * camera = nullptr;

    Model * testTriangle = nullptr;

    // material data
    ID3D11VertexShader * vertShader = nullptr;
    ID3D11PixelShader * pixelShader = nullptr;
    ID3D11InputLayout * inputLayout = nullptr;
    ID3D11Buffer * constantBuffer = nullptr;
  };

  extern DuckContext globalContext;

  struct testConstantBuffer {
    Matrix4 matrix{ tag::Identity{} };
  };


}