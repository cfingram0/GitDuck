#include "DuckGfx.h"
#include "DuckGfx_internal.h"
#include "DuckGfx_utils.h"

#define DEBUG_DX

namespace duckGfx {
  DuckContext globalContext;

  static_assert(sizeof(DuckContext::LightingData) == 720, "these must match");

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
    return hr == S_OK;
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
    rastDesc.MultisampleEnable = false;
    rastDesc.AntialiasedLineEnable = false;

    device->CreateRasterizerState(&rastDesc, rasterizerState);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.StencilEnable = false;

    device->CreateDepthStencilState(&depthStencilDesc, depthStencilState);


    ID3D11Buffer * constantBuffer = NULL;
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(Vec4);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    globalContext.pDevice->CreateBuffer(&cbDesc, NULL, buffer);

    return (*rasterizerState) && (*depthStencilState) && (*buffer);
  }

  static bool CreateMainColorPassData(ID3D11Device * device, 
                                      uint32_t width, 
                                      uint32_t height, 
                                      RenderTarget2D * renderTarget, 
                                      ID3D11RasterizerState ** rasterizerState, 
                                      ID3D11DepthStencilState ** depthStencilState, 
                                      ID3D11Buffer ** constantBuffer)
  {

    //create the render target
    D3D11_TEXTURE2D_DESC colorDesc;
    colorDesc.Width = width;
    colorDesc.Height = height;
    colorDesc.MipLevels = 1;
    colorDesc.ArraySize = 1;
    colorDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    colorDesc.SampleDesc.Count = 1;
    colorDesc.SampleDesc.Quality = 0;
    colorDesc.Usage = D3D11_USAGE_DEFAULT;
    colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    colorDesc.CPUAccessFlags = 0;
    colorDesc.MiscFlags = NULL;

    HRESULT hr = device->CreateTexture2D(&colorDesc, nullptr, &renderTarget->m_colorTargets[0]);
    if (hr != S_OK) {
      return false;
    }
    renderTarget->m_numColorTargets = 1;
    hr = device->CreateRenderTargetView(renderTarget->m_colorTargets[0], nullptr, &renderTarget->m_colorViews[0]);
    if (hr != S_OK) {
      return false;
    }
    hr = device->CreateShaderResourceView(renderTarget->m_colorTargets[0], nullptr, &renderTarget->m_colorSRV[0]);
    if (hr != S_OK) {
      return false;
    }

    D3D11_TEXTURE2D_DESC stencilDepthDesc;
    stencilDepthDesc.Width = width;
    stencilDepthDesc.Height = height;
    stencilDepthDesc.MipLevels = 1;
    stencilDepthDesc.ArraySize = 1;
    stencilDepthDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    stencilDepthDesc.SampleDesc.Count = 1;
    stencilDepthDesc.SampleDesc.Quality = 0;
    stencilDepthDesc.Usage = D3D11_USAGE_DEFAULT;
    stencilDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    stencilDepthDesc.CPUAccessFlags = 0;
    stencilDepthDesc.MiscFlags = NULL;

    hr = device->CreateTexture2D(&stencilDepthDesc, nullptr, &renderTarget->m_depthStencil);
    if (hr != S_OK) {
      return false;
    }
    hr = device->CreateDepthStencilView(renderTarget->m_depthStencil, nullptr, &renderTarget->m_depthStencilView);
    if (hr != S_OK) {
      return false;
    }

    // create the rasterizor state
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
    rastDesc.AntialiasedLineEnable = true;

    hr = device->CreateRasterizerState(&rastDesc, rasterizerState);
    if (hr != S_OK) {
      return false;
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.StencilEnable = false;

    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState);
    if (hr != S_OK) {
      return false;
    }

    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(Matrix4);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    hr = globalContext.pDevice->CreateBuffer(&cbDesc, NULL, constantBuffer);
    if (hr != S_OK) {
      return false;
    }

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
  

  static void UpdateLightingCb(Scene * theScene, DuckContext::LightingData * outBuffer) {
    assert(theScene && outBuffer);

    if (theScene->m_mainCamera) {
      Vec3 camTrans = theScene->m_mainCamera->m_transform.translation;
      outBuffer->cameraPos = Vec4(camTrans, 1);;
    }
    else {
      outBuffer->cameraPos = Vec4(0, 0, 0, 1);
    }

    outBuffer->lightColor = Vec4(theScene->m_lightColor, 1);
    outBuffer->lightDir = Vec4(theScene->m_lightDirection, 0);
    outBuffer->ambient = Vec4(theScene->m_ambientColor, 1);

    uint32_t numPointLights = theScene->m_pointLights.size() > 8 ? 8 : (uint32_t)theScene->m_pointLights.size();
    outBuffer->numLights[0] = numPointLights;
    for (uint32_t i = 0; i < numPointLights; ++i) {
      outBuffer->pointPos[i] = Vec4(theScene->m_pointLights[i]->m_position, 1);
      outBuffer->pointColor[i] = Vec4(theScene->m_pointLights[i]->m_color, theScene->m_pointLights[i]->m_intensity);
    }

    uint32_t numSpotLights = theScene->m_spotLights.size() > 8 ? 8 : (uint32_t)theScene->m_spotLights.size();
    outBuffer->numLights[1] = numSpotLights;
    for (uint32_t i = 0; i < numSpotLights; ++i) {
      outBuffer->spotPos[i].spotPos = theScene->m_spotLights[i]->m_position;
      outBuffer->spotPos[i].spotCosInnerAngle = theScene->m_spotLights[i]->m_cosInnerAngle;

      outBuffer->spotDir[i].spotDirection = theScene->m_spotLights[i]->m_direction;
      outBuffer->spotDir[i].spotCosOuterAngle = theScene->m_spotLights[i]->m_cosOuterAngle;

      outBuffer->spotInten[i].spotColor = theScene->m_spotLights[i]->m_color;
      outBuffer->spotInten[i].spotIntensity = theScene->m_spotLights[i]->m_intensity;
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
    if (!CreateToBackBufferRenderState(globalContext.pDevice, &globalContext.toBackbufferRS, &globalContext.toBackbufferDSS, &globalContext.toBackBufferPassCb)) {
      return false;
    }

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

    //create the debug draw data
    globalContext.debugLineMesh = new Mesh();
    GenerateDebugLineMesh(globalContext.debugLineMesh);
    globalContext.debugDrawMaterial = reinterpret_cast<Material*>(GenerateDebugDrawMaterial());
    globalContext.debugMaterialInstance = new MaterialInstance(globalContext.debugDrawMaterial);

    globalContext.pMainColorRt = new RenderTarget2D();
    if (!CreateMainColorPassData(globalContext.pDevice,
                                 globalContext.width,
                                 globalContext.height,
                                 globalContext.pMainColorRt,
                                 &globalContext.toMainColorRS,
                                 &globalContext.toMainColorDSS,
                                 &globalContext.toMainColorCb)) {
      return false;
    }

    globalContext.backBufferQuad = new Mesh();
    GenerateQuad(globalContext.pDevice, globalContext.backBufferQuad);

    globalContext.toBackBufferMat = new Material();
    CreateToBackbufferMaterial(globalContext.pDevice, globalContext.toBackBufferMat);
    globalContext.toBackBufferMatInstance = new MaterialInstance(globalContext.toBackBufferMat);

    CD3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
    samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    globalContext.pDevice->CreateSamplerState(&samplerDesc, &globalContext.toBackBufferSamplerSate);

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

    //------------------------------------------------------------------------------------------------------------
    // pass 1 : clear back buffer
    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"Clear Render Targets");
    static int counter = 0;
    // clear the back buffer, alternate colors so we know the back buffer is being cleared
    float color[4] = { 0.0f, 0.02f, 0.32f, 0.0f };
    float color2[4] = { 0.0f, 0.21f, 0.133f, 0.0f };
    bool useColor2 = false;

    if (counter / 60 == 1) {
      useColor2 = true;
      if (counter == 119) {
        counter = 0;
      }
    }

    float backBufferColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    globalContext.pImmediateContext->ClearRenderTargetView(globalContext.pBackBufferRt->m_colorViews[0], backBufferColor);

    globalContext.pImmediateContext->ClearRenderTargetView(globalContext.pMainColorRt->m_colorViews[0], useColor2 ? color2 : color);
    globalContext.pImmediateContext->ClearDepthStencilView(globalContext.pMainColorRt->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    if (globalContext.annotator)
      globalContext.annotator->EndEvent();


    //------------------------------------------------------------------------------------------------------------
    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"Main Color");

    // pass 2 : render to back buffer
    BindRenderTarget2D(globalContext.pImmediateContext, *globalContext.pMainColorRt);

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
    globalContext.pImmediateContext->RSSetState(globalContext.toMainColorRS);
    globalContext.pImmediateContext->OMSetDepthStencilState(globalContext.toMainColorDSS, 0);


    //set up pass constant buffer
    Matrix4 cam = Matrix4{ tag::Identity{} };
    if (globalContext.theScene.m_mainCamera)
      cam = globalContext.theScene.m_mainCamera->m_viewProj;

    D3D11_MAPPED_SUBRESOURCE passCb;
    globalContext.pImmediateContext->Map(globalContext.toMainColorCb, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &passCb);
    memcpy(passCb.pData, cam.v, sizeof(Matrix4));
    globalContext.pImmediateContext->Unmap(globalContext.toMainColorCb, 0);
    globalContext.pImmediateContext->VSSetConstantBuffers(1, 1, &globalContext.toMainColorCb);

    // set up lighting constant buffer
    UpdateLightingCb(&globalContext.theScene, &globalContext.lightingDataBuffer);

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


    //------------------------------------------------------------------------------------------------------------
    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"Debug Draw");

    // pass 3 : debug draw (use same render target + state as color pass
    BindMaterial(globalContext.pImmediateContext, globalContext.debugDrawMaterial, MaterialTechniqueID::kColor);
    BindMesh(globalContext.pImmediateContext, *globalContext.debugLineMesh);
    for (uint32_t lineIter = 0; lineIter < globalContext.theScene.m_debugLines.size(); ++lineIter) {
      //calculate the transform
      DebugLine & line = globalContext.theScene.m_debugLines[lineIter];
      TransformSRT lineTransform{ tag::NoInit {} };
      lineTransform.translation = line.p0;
      lineTransform.scale = Vec3(Length(line.p1 - line.p0), 1, 1);
      Vec3 v1 = Vec3(1.0f, 0, 0);
      Vec3 v2 = Normalize(line.p1 - line.p0);
      Vec3 axis = Cross(v1, v2);
      if (IsEqual(axis, Vec3(tag::Zero{}))) {
        if (Dot(v1, v2) >= 0) {
          lineTransform.rotation = Quaternion(tag::Identity{});
        }
        else {
          lineTransform.rotation = Quaternion(3.14159f, Vec4(0, 1, 0, 0));
        }
      } 
      else {
        axis = Normalize(axis);
        float angle = std::acos(Dot(v1, v2));
        lineTransform.rotation = Quaternion(angle, axis);
      }

      globalContext.debugMaterialInstance->SetParameter("gTransform", lineTransform.CalcMatrix());
      globalContext.debugMaterialInstance->SetParameter("color", Vec4(line.color, 1));
      MapMaterialInstanceData(globalContext.pImmediateContext, globalContext.debugDrawMaterial, globalContext.debugMaterialInstance, MaterialTechniqueID::kColor);
      globalContext.pImmediateContext->DrawIndexed(2, 0, 0);
    }
    globalContext.theScene.m_debugLines.clear();

    if (globalContext.annotator)
      globalContext.annotator->EndEvent();

    //------------------------------------------------------------------------------------------------------------
    if (globalContext.annotator)
      globalContext.annotator->BeginEvent(L"To Backbuffer");
    BindRenderTarget2D(globalContext.pImmediateContext, *globalContext.pBackBufferRt);

    globalContext.pImmediateContext->RSSetState(globalContext.toBackbufferRS);
    globalContext.pImmediateContext->OMSetDepthStencilState(globalContext.toBackbufferDSS, 0);

    BindMaterial(globalContext.pImmediateContext, globalContext.toBackBufferMat, MaterialTechniqueID::kColor);
    BindMesh(globalContext.pImmediateContext, *globalContext.backBufferQuad);

    globalContext.pImmediateContext->PSSetSamplers(0, 1, &globalContext.toBackBufferSamplerSate);
    globalContext.pImmediateContext->PSSetShaderResources(0, 1, &globalContext.pMainColorRt->m_colorSRV[0]);

    globalContext.pImmediateContext->DrawIndexed(6, 0, 0);


    if (globalContext.annotator)
      globalContext.annotator->EndEvent();

    ID3D11SamplerState * nullSampler = nullptr;
    ID3D11ShaderResourceView * nullSRV = nullptr;
    globalContext.pImmediateContext->PSSetSamplers(0, 1, &nullSampler);
    globalContext.pImmediateContext->PSSetShaderResources(0, 1, &nullSRV);


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

    globalContext.lightingDataCb->Release();

    delete globalContext.pBackBufferRt;
    globalContext.pBackBufferRt = nullptr;

    globalContext.toBackbufferRS->Release();
    globalContext.toBackbufferDSS->Release();
    globalContext.toBackBufferPassCb->Release();

    delete globalContext.pMainColorRt;
    globalContext.pMainColorRt = nullptr;
    globalContext.toMainColorRS->Release();
    globalContext.toMainColorDSS->Release();
    globalContext.toMainColorCb->Release();

    delete globalContext.debugLineMesh;
    delete globalContext.debugMaterialInstance;
    delete globalContext.debugDrawMaterial;


    globalContext.annotator->Release();
  }
}