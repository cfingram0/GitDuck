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
  static bool CreateToBackBufferRenderState(ID3D11Device * device, 
                                            ID3D11RasterizerState ** rasterizerState,
                                            ID3D11DepthStencilState ** depthStencilState, 
                                            ID3D11Buffer ** buffer) {
    D3D11_RASTERIZER_DESC rastDesc;
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = true;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0;
    rastDesc.SlopeScaledDepthBias = 0;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.ScissorEnable = true;
    rastDesc.MultisampleEnable = true;
    rastDesc.AntialiasedLineEnable = false;

    device->CreateRasterizerState(&rastDesc, rasterizerState);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.StencilEnable = false;

    device->CreateDepthStencilState(&depthStencilDesc, depthStencilState);


    ID3D11Buffer * constantBuffer = NULL;
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(Matrix4);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    globalContext.pDevice->CreateBuffer(&cbDesc, NULL, buffer);

    return (*rasterizerState) && (*depthStencilState) && (*buffer);
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
    sc.BufferCount = 2;
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
  

  bool Init(HWND windowHandle, uint32_t width, uint32_t height) {
    IDXGIAdapter1 * chosenAdaptor = nullptr;
    if (!PickAdaptor(&chosenAdaptor)) {
      return false;
    }

    globalContext.width = width;
    globalContext.height = height;

    if (!CreateSwapChain(windowHandle, globalContext.width, globalContext.height, chosenAdaptor, &globalContext.pSwapChain, &globalContext.pDevice, &globalContext.pImmediateContext)) {
      return false;
    }
    chosenAdaptor->Release();

    // create the back buffer rt
    globalContext.pBackBufferRt = new RenderTarget2D();
    if (!CreateBackBufferRT(globalContext.pSwapChain, globalContext.pBackBufferRt)) {
      return false;
    }

    // create the render state for the backbuffer
    CreateToBackBufferRenderState(globalContext.pDevice, &globalContext.toBackbufferRS, &globalContext.toBackbufferDSS, &globalContext.toBackBufferPassCb);

    //light data
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(DuckContext::LightingData);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;
    globalContext.pDevice->CreateBuffer(&cbDesc, NULL, &globalContext.lightingDataCb);

    globalContext.pImmediateContext->QueryInterface(&globalContext.annotator);
    return true;
  }
  
  void Render() {
    // update the mesh uniform data 
    for (uint32_t i = 0; i < globalContext.theScene.m_models.size(); ++i) {
      Model* model = globalContext.theScene.m_models[i];
      model->m_matInstance->SetParameter("gTransform", model->m_transform.CalcMatrix());

      Matrix4 invTrans = model->m_transform.CalcInvMatrix();
      invTrans.Transpose();
      model->m_matInstance->SetParameter("gNormalTransform", invTrans);
    }

    // pass 1 : clear back buffer
    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"Clear Backbuffer");
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

    if (globalContext.annotator)
      globalContext.annotator->EndEvent();

    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"Render To Backbuffer");

    // pass 2 : render to back buffer
    BindRenderTarget2D(globalContext.pImmediateContext, *globalContext.pBackBufferRt);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)globalContext.width;
    viewport.Height = (float)globalContext.height;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    
    D3D11_RECT rect;
    rect.top = 0;
    rect.left = 0;
    rect.bottom = globalContext.height;
    rect.right = globalContext.width;

    // pass data
    globalContext.pImmediateContext->RSSetViewports(1, &viewport);
    globalContext.pImmediateContext->RSSetScissorRects(1, &rect);
    globalContext.pImmediateContext->RSSetState(globalContext.toBackbufferRS);
    globalContext.pImmediateContext->OMSetDepthStencilState(globalContext.toBackbufferDSS, 0);


    //set up pass constant buffer
    Matrix4 cam = Matrix4{ tag::Identity{} };
    if (globalContext.theScene.m_mainCamera)
      cam = globalContext.theScene.m_mainCamera->m_viewProj;

    D3D11_MAPPED_SUBRESOURCE passCb;
    globalContext.pImmediateContext->Map(globalContext.toBackBufferPassCb, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &passCb);
    memcpy(passCb.pData, cam.v, sizeof(Matrix4));
    globalContext.pImmediateContext->Unmap(globalContext.toBackBufferPassCb, 0);
    globalContext.pImmediateContext->VSSetConstantBuffers(1, 1, &globalContext.toBackBufferPassCb);

    // set up lighting constant buffer
    if (globalContext.theScene.m_mainCamera) {
      Vec3 camTrans = globalContext.theScene.m_mainCamera->m_transform.translation;
      globalContext.lightingDataBuffer.cameraPos = Vec4(camTrans, 1);;
    }
    else {
      globalContext.lightingDataBuffer.cameraPos = Vec4(0, 0, 0, 1);
    }

    globalContext.lightingDataBuffer.lightColor = Vec4(globalContext.theScene.m_lightColor, 1);
    globalContext.lightingDataBuffer.lightDir = Vec4(globalContext.theScene.m_lightDirection, 0);
    globalContext.lightingDataBuffer.ambient = Vec4(globalContext.theScene.m_ambientColor, 1);
    
    globalContext.lightingDataBuffer.pointPos = Vec4(globalContext.theScene.m_pointLights[0]->m_position, 1);
    globalContext.lightingDataBuffer.pointColor = Vec4(globalContext.theScene.m_pointLights[0]->m_color, globalContext.theScene.m_pointLights[0]->m_intensity);

    globalContext.lightingDataBuffer.spotPos = globalContext.theScene.m_spotLights[0]->m_position;
    globalContext.lightingDataBuffer.spotDirection = globalContext.theScene.m_spotLights[0]->m_direction;
    globalContext.lightingDataBuffer.spotColor = globalContext.theScene.m_spotLights[0]->m_color;
    globalContext.lightingDataBuffer.spotIntensity = globalContext.theScene.m_spotLights[0]->m_intensity;
    globalContext.lightingDataBuffer.spotCosInnerAngle = globalContext.theScene.m_spotLights[0]->m_cosInnerAngle;
    globalContext.lightingDataBuffer.spotCosOuterAngle = globalContext.theScene.m_spotLights[0]->m_cosOuterAngle;

    globalContext.pImmediateContext->Map(globalContext.lightingDataCb, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &passCb);
    memcpy(passCb.pData, globalContext.lightingDataBuffer.data, sizeof(DuckContext::LightingData));
    globalContext.pImmediateContext->Unmap(globalContext.lightingDataCb, 0);
    globalContext.pImmediateContext->PSSetConstantBuffers(1, 1, &globalContext.lightingDataCb);



    for (uint32_t modelIter = 0; modelIter < globalContext.theScene.m_models.size(); ++modelIter) {
      Model * model = globalContext.theScene.m_models[modelIter];

      // material data
      BindMaterial(globalContext.pImmediateContext, model->m_matInstance->m_material, MaterialTechniqueID::kColor);

      //set the mesh
      BindMesh(globalContext.pImmediateContext, *model->m_theMesh);

      // update the constant buffer data
      MapMaterialInstanceData(globalContext.pImmediateContext, model->m_matInstance->m_material, model->m_matInstance, MaterialTechniqueID::kColor);

      globalContext.pImmediateContext->DrawIndexed(model->m_theMesh->idxCount, 0, 0);
    }


    if (globalContext.annotator)
      globalContext.annotator->EndEvent();

    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"Swap");
    // flip the swap chain, with vsync
    globalContext.pSwapChain->Present(1, 0);
    if (globalContext.annotator)
      globalContext.annotator->EndEvent();


    counter++;
  }

  void Shutdown() {
    // release all of our data
    globalContext.pImmediateContext->Release();
    globalContext.pDevice->Release();
    globalContext.pSwapChain->Release();

    delete globalContext.pBackBufferRt;
    globalContext.pBackBufferRt = nullptr;

    globalContext.toBackbufferRS->Release();
    globalContext.toBackbufferDSS->Release();
    globalContext.toBackBufferPassCb->Release();

    globalContext.annotator->Release();
  }
}