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

  struct DuckContext {
    IDXGISwapChain * pSwapChain = nullptr;
    ID3D11Device * pDevice = nullptr;
    ID3D11DeviceContext * pImmediateContext = nullptr;
    ID3D11RenderTargetView * pBackBufferRenderTargetView = nullptr;
    ID3D11Texture2D * depthBufferTex = NULL;
    ID3D11DepthStencilView * depthStencilView = NULL;


    ID3D11Buffer * vertBuffer = nullptr;
    ID3D11Buffer * idxBuffer = nullptr;
    ID3D11InputLayout * inputLayout = nullptr;
    ID3D11VertexShader * vertShader = nullptr;
    ID3D11PixelShader * pixelShader = nullptr;
    ID3D11RasterizerState * rasterizorState = nullptr;
    ID3D11DepthStencilState * depthStencilState = nullptr;
  };

  extern DuckContext globalContext;

  struct testVert {
    Vec3 position;
    Vec3 color;
  };
}