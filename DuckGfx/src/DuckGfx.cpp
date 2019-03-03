#include "DuckGfx.h"
#include "DuckGfx_internal.h"

namespace duckGfx {
  DuckContext globalContext;

  bool Init(HWND windowHandle) {
    // get the adaptors and pick the one with the most dedicated ram
    IDXGIFactory1 * pFactory = NULL;
    CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

    IDXGIAdapter1 * pAdaptor = NULL;
    std::vector<IDXGIAdapter1 *> adaptorArray;
    for (uint32_t i = 0; pFactory->EnumAdapters1(i, &pAdaptor) != DXGI_ERROR_NOT_FOUND; ++i) {
      adaptorArray.push_back(pAdaptor);
    }

    uint32_t adapterWithMostRAM = 0;
    SIZE_T dedicatedMemory = 0;
    for (uint32_t i = 0; i < adaptorArray.size(); ++i) {
      DXGI_ADAPTER_DESC1 desc;
      adaptorArray[i]->GetDesc1(&desc);
      if (desc.DedicatedVideoMemory > dedicatedMemory) {
        dedicatedMemory = desc.DedicatedVideoMemory;
        adapterWithMostRAM = 0;
      }

      WriteToOutput("%s\n", desc.Description);
    }

    // create the swap chain
    DXGI_SWAP_CHAIN_DESC sc;
    ZeroMemory(&sc, sizeof(sc));
    sc.BufferCount = 1;
    sc.BufferDesc.Width = 1600;
    sc.BufferDesc.Height = 900;
    sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sc.BufferDesc.RefreshRate.Numerator = 60;
    sc.BufferDesc.RefreshRate.Denominator = 1;
    sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc.OutputWindow = windowHandle;
    sc.SampleDesc.Count = 1;
    sc.SampleDesc.Quality = 0;
    sc.Windowed = true;

    D3D_FEATURE_LEVEL featureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
    uint32_t numLevelsRequested = 1;
    D3D_FEATURE_LEVEL featureLevelsSupported;
    

    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
      D3D_DRIVER_TYPE_HARDWARE,
      NULL,
      0,
      &featureLevelsRequested,
      numLevelsRequested,
      D3D11_SDK_VERSION,
      &sc,
      &globalContext.pSwapChain,
      &globalContext.pDevice,
      &featureLevelsSupported,
      &globalContext.pImmediateContext);

    if (FAILED(hr)) {
      return false;
    }

    //get the back buffer, create a render target view, bind the view
    ID3D11Texture2D * pBackBuffer;
    hr = globalContext.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr)) {
      return false;
    }

    //create the render target view
   
    hr = globalContext.pDevice->CreateRenderTargetView(pBackBuffer, NULL, &globalContext.pBackBufferRenderTargetView);

    //bind the view
    globalContext.pImmediateContext->OMSetRenderTargets(1, &globalContext.pBackBufferRenderTargetView, NULL);

    // release intermediate objects
    for (size_t i = 0; i < adaptorArray.size(); ++i) {
      adaptorArray[i]->Release();
    }
    pFactory->Release();

    // generate buffer for hello world triangle
    testVert verts[] = { Vec3(-0.5f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), 
                         Vec3(0.5f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), 
                         Vec3(0.5f, 0.5f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)};

    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(testVert) * 3;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = verts;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    ID3D11Buffer * vertBuffer;
    hr = globalContext.pDevice->CreateBuffer(&desc, &initData, &vertBuffer);
    if (FAILED(hr)) {
      return false;
    }
    globalContext.vertBuffer = vertBuffer;

    uint32_t indices[] = {0, 1, 2};
    D3D11_BUFFER_DESC desc_idx;
    desc_idx.Usage = D3D11_USAGE_DEFAULT;
    desc_idx.ByteWidth = sizeof(uint32_t) * 3;
    desc_idx.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc_idx.CPUAccessFlags = 0;
    desc_idx.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData_idx;
    initData_idx.pSysMem = indices;
    initData_idx.SysMemPitch = 0;
    initData_idx.SysMemSlicePitch = 0;

    ID3D11Buffer * idxBuffer;
    hr = globalContext.pDevice->CreateBuffer(&desc_idx, &initData_idx, &idxBuffer);
    globalContext.idxBuffer = idxBuffer;

    // load the shaders
    std::vector<uint8_t> bytes;
    bool success = ReadAllBytes("TestVertexShader.cso", &bytes);
    if (!success) {
      return false;
    }

    ID3D11VertexShader * vertShader = nullptr;
    globalContext.pDevice->CreateVertexShader(bytes.data(), bytes.size(), nullptr, &vertShader);
    if (!vertShader) {
      return false;
    }
    globalContext.vertShader = vertShader;


    ID3D11InputLayout * pLayout;
    D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = globalContext.pDevice->CreateInputLayout(layout, 2, bytes.data(), bytes.size(), &pLayout);
    if (FAILED(hr)) {
      return false;
    }
    globalContext.inputLayout = pLayout;


    ID3D11PixelShader * pixelShader;
    success = ReadAllBytes("TestPixelShader.cso", &bytes);
    if (!success) {
      return false;
    }
    globalContext.pDevice->CreatePixelShader(bytes.data(), bytes.size(), nullptr, &pixelShader);
    if (!pixelShader) {
      return false;
    }
    globalContext.pixelShader = pixelShader;


    ID3D11RasterizerState * rasterizorState;
    D3D11_RASTERIZER_DESC rastDesc;
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = true;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0;
    rastDesc.SlopeScaledDepthBias = 0;
    rastDesc.DepthClipEnable = 0;
    rastDesc.ScissorEnable = false;
    rastDesc.MultisampleEnable = false;
    rastDesc.AntialiasedLineEnable = false;

    globalContext.pDevice->CreateRasterizerState(&rastDesc, &rasterizorState);
    globalContext.rasterizorState = rasterizorState;


    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.StencilEnable = false;

    ID3D11DepthStencilState * depthStencilState;
    globalContext.pDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    globalContext.depthStencilState = depthStencilState;

    return true;
  }
  
  void Render() {
    static int counter = 0;
    // clear the back buffer, alternate colors so we know the back buffer is being cleared
    float color[4] = { 0.0f, 0.2f, 0.6f, 0.0f };
    float color2[4] = { 0.0f, 0.5f, 0.4f, 0.0f };
    if (counter / 60 == 1) {
      globalContext.pImmediateContext->ClearRenderTargetView(globalContext.pBackBufferRenderTargetView, color2);
      if (counter == 119) {
        counter = 0;
      }
    }
    else {
      globalContext.pImmediateContext->ClearRenderTargetView(globalContext.pBackBufferRenderTargetView, color);
    }

    // set our default viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 1600;
    viewport.Height = 900;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    globalContext.pImmediateContext->RSSetViewports(1, &viewport);


    // hey look, the place where real rendering happens
    uint32_t stride = sizeof(testVert);
    uint32_t offset = 0;
    globalContext.pImmediateContext->IASetVertexBuffers(0, 1, &globalContext.vertBuffer, &stride, &offset);
    globalContext.pImmediateContext->IASetIndexBuffer(globalContext.idxBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    globalContext.pImmediateContext->IASetInputLayout(globalContext.inputLayout);
    globalContext.pImmediateContext->VSSetShader(globalContext.vertShader, nullptr, 0);
    globalContext.pImmediateContext->PSSetShader(globalContext.pixelShader, nullptr, 0);
    globalContext.pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    globalContext.pImmediateContext->RSSetState(globalContext.rasterizorState);
    globalContext.pImmediateContext->OMSetDepthStencilState(globalContext.depthStencilState, 0);

    globalContext.pImmediateContext->DrawIndexed(3, 0, 0);



    // flip the swap chain, with vsync
    globalContext.pSwapChain->Present(1, 0);
    counter++;
  }

  void ShutDown() {
    // release all of our data
    globalContext.pBackBufferRenderTargetView->Release();
    globalContext.pImmediateContext->Release();
    globalContext.pDevice->Release();
    globalContext.pSwapChain->Release();

  }
}