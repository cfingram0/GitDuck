#pragma once
#include <d3d11.h>
#include <cstdio>
#include <cassert>
#include <cstdint>
#include <vector>

#include "debug.h"
#include "dmath.h"
#include "io.h"

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

    // mesh data
    ID3D11Buffer * vertBuffer = nullptr;
    ID3D11Buffer * idxBuffer = nullptr;
    ID3D11InputLayout * inputLayout = nullptr;

    // material data
    ID3D11VertexShader * vertShader = nullptr;
    ID3D11PixelShader * pixelShader = nullptr;
    ID3D11Buffer * constantBuffer = nullptr;
  };

  extern DuckContext globalContext;

  struct testVert {
    Vec3 position;
    Vec3 color;
  };

  struct testConstantBuffer {
    Matrix4 matrix{ tag::Identity{} };
  };


}