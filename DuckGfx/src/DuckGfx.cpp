#include "DuckGfx.h"
#include "DuckGfx_internal.h"

#define DEBUG_DX

namespace duckGfx {
  DuckContext globalContext;

  RenderTarget2D::~RenderTarget2D() {
    if (m_depthStencil) {
      m_depthStencil->Release();
      m_depthStencilView->Release();
    }

    for (uint8_t i = 0; i < m_numColorTargets; ++i) {
      m_colorTargets[i]->Release();
      m_colorViews[i]->Release();
    }
  }

  void BindRenderTarget2D(ID3D11DeviceContext * context, const RenderTarget2D & rt) {
    context->OMSetRenderTargets(rt.m_numColorTargets, rt.m_colorViews, rt.m_depthStencilView);
  }

  static bool CreateBackBufferRT(IDXGISwapChain * swapChain, RenderTarget2D * output) {
    //get the back buffer, create a render target view, bind the view
    HRESULT hr = globalContext.pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&output->m_colorTargets[0]);
    if (FAILED(hr)) {
      return false;
    }

    output->m_numColorTargets = 1;
    //create the render target view
    hr = globalContext.pDevice->CreateRenderTargetView(output->m_colorTargets[0], NULL, output->m_colorViews);

    D3D11_TEXTURE2D_DESC backBufferDesc;
    output->m_colorTargets[0]->GetDesc(&backBufferDesc);

    //create the stencil depth buffer
    D3D11_TEXTURE2D_DESC stencilDepthDesc;
    stencilDepthDesc.Width = backBufferDesc.Width;
    stencilDepthDesc.Height = backBufferDesc.Height;
    stencilDepthDesc.MipLevels = 1;
    stencilDepthDesc.ArraySize = 1;
    stencilDepthDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    stencilDepthDesc.SampleDesc.Count = 1;
    stencilDepthDesc.SampleDesc.Quality = 0;
    stencilDepthDesc.Usage = D3D11_USAGE_DEFAULT;
    stencilDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    stencilDepthDesc.CPUAccessFlags = 0;
    stencilDepthDesc.MiscFlags = NULL;

    globalContext.pDevice->CreateTexture2D(&stencilDepthDesc, NULL, &output->m_depthStencil);
    globalContext.pDevice->CreateDepthStencilView(output->m_depthStencil, NULL, &output->m_depthStencilView);
    return true;
  }

  static bool PickAdaptor(IDXGIAdapter1 ** outAdaptor) {
    IDXGIAdapter1 * toReturn = nullptr;

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

    toReturn = adaptorArray[adapterWithMostRAM];
    toReturn->AddRef();
    *outAdaptor = toReturn;

    // release intermediate objects
    for (size_t i = 0; i < adaptorArray.size(); ++i) {
      adaptorArray[i]->Release();
    }
    pFactory->Release();

    return true;
  }

  static bool CreateSwapChain(HWND windowHandle, 
                              uint32_t width, 
                              uint32_t height, 
                              IDXGIAdapter1 * adaptor, 
                              IDXGISwapChain **outSwapChain,
                              ID3D11Device **outDevice,
                              ID3D11DeviceContext **outImmediateContext)
  {  
    // create the swap chain
    DXGI_SWAP_CHAIN_DESC sc;
    ZeroMemory(&sc, sizeof(sc));
    sc.BufferCount = 1;
    sc.BufferDesc.Width = width;
    sc.BufferDesc.Height = height;
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

    HRESULT hr = D3D11CreateDeviceAndSwapChain(adaptor,
      D3D_DRIVER_TYPE_UNKNOWN,
      NULL,
#ifdef DEBUG_DX
      D3D11_CREATE_DEVICE_DEBUG,
#else
      NULL,
#endif
      &featureLevelsRequested,
      numLevelsRequested,
      D3D11_SDK_VERSION,
      &sc,
      outSwapChain,
      outDevice,
      &featureLevelsSupported,
      outImmediateContext);

    if (FAILED(hr)) {
      return false;
    }
    else {
      return true;
    }
  }

  bool Init(HWND windowHandle) {
    IDXGIAdapter1 * chosenAdaptor = nullptr;
    if (!PickAdaptor(&chosenAdaptor)) {
      return false;
    }

    if (!CreateSwapChain(windowHandle, 1600, 900, chosenAdaptor, &globalContext.pSwapChain, &globalContext.pDevice, &globalContext.pImmediateContext)) {
      return false;
    }
    chosenAdaptor->Release();

    // create the back buffer rt
    globalContext.pBackBufferRt = new RenderTarget2D();
    if (!CreateBackBufferRT(globalContext.pSwapChain, globalContext.pBackBufferRt)) {
      return false;
    }

    // generate buffer for hello world triangle
    testVert verts[] = { Vec3(-0.5f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), 
                         Vec3(0.5f, 0.0f,  0.0f), Vec3(0.0f, 1.0f, 0.0f), 
                         Vec3(0.5f, 0.5f,  0.0f), Vec3(0.0f, 0.0f, 1.0f)};

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
    HRESULT hr = globalContext.pDevice->CreateBuffer(&desc, &initData, &vertBuffer);
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
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.ScissorEnable = false;
    rastDesc.MultisampleEnable = false;
    rastDesc.AntialiasedLineEnable = false;

    globalContext.pDevice->CreateRasterizerState(&rastDesc, &rasterizorState);
    globalContext.rasterizorState = rasterizorState;


    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.StencilEnable = false;

    ID3D11DepthStencilState * depthStencilState;
    globalContext.pDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    globalContext.depthStencilState = depthStencilState;


    ID3D11Buffer * constantBuffer = NULL;
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(testConstantBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    hr = globalContext.pDevice->CreateBuffer(&cbDesc, NULL, &constantBuffer);
    if (FAILED(hr)) {
      return false;
    }
    globalContext.constantBuffer = constantBuffer;

    // set up the camera
    globalContext.camera = new Camera();
    globalContext.camera->m_AspectRatio = 16.0f / 9.0f;
    globalContext.camera->m_near = 1.0f;
    globalContext.camera->m_far = 10.0f;
    globalContext.camera->m_wFov = (90.0f * 3.14f) / 180.0f;
    globalContext.camera->m_transform.translation = Vec3(0, 2, 0);
    globalContext.camera->RefreshProjMatrix();
    return true;
  }
  
  void Render() {
    


    // pass 1 : clear back buffer
    RenderTarget2D * backBuffer = globalContext.pBackBufferRt;
    BindRenderTarget2D(globalContext.pImmediateContext, *globalContext.pBackBufferRt);

    static int counter = 0;
    // clear the back buffer, alternate colors so we know the back buffer is being cleared
    float color[4] = { 0.0f, 0.2f, 0.6f, 0.0f };
    float color2[4] = { 0.0f, 0.5f, 0.4f, 0.0f };

    globalContext.pImmediateContext->ClearDepthStencilView(backBuffer->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    if (counter / 60 == 1) {
      globalContext.pImmediateContext->ClearRenderTargetView(backBuffer->m_colorViews[0], color2);
      if (counter == 119) {
        counter = 0;
      }
    }
    else {
      globalContext.pImmediateContext->ClearRenderTargetView(backBuffer->m_colorViews[0], color);
    }


    // pass 2 : render to back buffer
    BindRenderTarget2D(globalContext.pImmediateContext, *globalContext.pBackBufferRt);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 1600;
    viewport.Height = 900;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    globalContext.pImmediateContext->RSSetViewports(1, &viewport);
    
    // pass data
    globalContext.pImmediateContext->RSSetState(globalContext.rasterizorState);
    globalContext.pImmediateContext->OMSetDepthStencilState(globalContext.depthStencilState, 0);
    globalContext.camera->RefreshProjView();


    // hey look, the place where real rendering happens
    uint32_t stride = sizeof(testVert);
    uint32_t offset = 0;

    // mesh data
    globalContext.pImmediateContext->IASetVertexBuffers(0, 1, &globalContext.vertBuffer, &stride, &offset);
    globalContext.pImmediateContext->IASetIndexBuffer(globalContext.idxBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    globalContext.pImmediateContext->IASetInputLayout(globalContext.inputLayout);
    globalContext.pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // material data
    globalContext.pImmediateContext->VSSetShader(globalContext.vertShader, nullptr, 0);
    globalContext.pImmediateContext->PSSetShader(globalContext.pixelShader, nullptr, 0);

    Matrix4 mymatrix(tag::Identity{});
    TransformSRT worldT(Vec3(2, 1, 1), Quaternion((45.0f * 3.14f) / 180.0f, Vec3(0, 0, 1)), Vec3(0, 3, -5));
    Matrix4 world = worldT.CalcMatrix();

    mymatrix = globalContext.camera->m_viewProj * world;

    D3D11_MAPPED_SUBRESOURCE subresource;
    ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    globalContext.pImmediateContext->Map(globalContext.constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    memcpy(subresource.pData, &mymatrix.v[0], sizeof(Matrix4));
    globalContext.pImmediateContext->Unmap(globalContext.constantBuffer, 0);
    globalContext.pImmediateContext->VSSetConstantBuffers(0, 1, &globalContext.constantBuffer);

    globalContext.pImmediateContext->DrawIndexed(3, 0, 0);


    // flip the swap chain, with vsync
    globalContext.pSwapChain->Present(1, 0);
    counter++;
  }

  void ShutDown() {
    // release all of our data
    globalContext.pImmediateContext->Release();
    globalContext.pDevice->Release();
    globalContext.pSwapChain->Release();

    delete globalContext.pBackBufferRt;
    globalContext.pBackBufferRt = nullptr;

    delete globalContext.camera;
    globalContext.camera = nullptr;

    globalContext.vertBuffer->Release();
    globalContext.idxBuffer->Release();
    globalContext.inputLayout->Release();
    globalContext.vertShader->Release();
    globalContext.pixelShader->Release();
    globalContext.rasterizorState->Release();
    globalContext.depthStencilState->Release();
    globalContext.constantBuffer->Release();
  }
}