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

  void BindMesh(ID3D11DeviceContext * context, const Mesh & mesh) {
    // hey look, the place where real rendering happens
    uint32_t stride = mesh.vertSize;
    uint32_t offset = 0;

    // mesh data
    context->IASetVertexBuffers(0, 1, &mesh.vertBuffer, &stride, &offset);
    context->IASetIndexBuffer(mesh.idxBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(mesh.topology);
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

  static uint32_t VertSize(const VertexFormat & fmt) {
    uint32_t sizeAccum = 0;
    if (fmt.hasPosition) {
      sizeAccum += sizeof(Vec3);
    }
    if (fmt.hasNormals) {
      sizeAccum += sizeof(Vec3);
    }
    if (fmt.hasTangentFrame) {
      sizeAccum += 2 * sizeof(Vec3);
    }
    if (fmt.hasBlendWeightsAndIndices) {
      sizeAccum += 4 * (sizeof(float) + sizeof(uint32_t));
    }
    if (fmt.numUVsets) {
      sizeAccum += fmt.numUVsets * sizeof(Vec2);
    }
    if (fmt.numColorSets) {
      sizeAccum += fmt.numColorSets * sizeof(Vec4);
    }
    return sizeAccum;
  }


  static bool GenerateDebugTriangle(Mesh* output) {
    output->format.hasPosition = 1;
    output->format.numColorSets = 1;

    // generate buffer for hello world triangle
    struct { 
      Vec3 pos; 
      Vec4 color; 
    } vertData[] = { Vec3(-0.5f, 0.0f, 0.0f), Vec4(1.0f, 0.0f, 0.0f, 1),
                     Vec3(0.5f, 0.0f,  0.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f),
                     Vec3(0.5f, 0.5f,  0.0f), Vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    const uint32_t numVerts = 3;
    const uint32_t vertSize = VertSize(output->format);
    output->vertSize = vertSize;

    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = vertSize * numVerts;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = vertData;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    ID3D11Buffer * vertBuffer;
    HRESULT hr = globalContext.pDevice->CreateBuffer(&desc, &initData, &vertBuffer);
    if (FAILED(hr)) {
      return false;
    }
    output->vertBuffer = vertBuffer;

    uint32_t indices[] = { 0, 1, 2 };
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
    if (FAILED(hr)) {
      return false;
    }

    output->idxBuffer = idxBuffer;
    output->idxCount = 3;
    output->topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    return true;
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

    // create the mesh
    Mesh * newMesh = new Mesh();
    GenerateDebugTriangle(newMesh);
    globalContext.testTriangle = new Model();
    globalContext.testTriangle->m_theMesh = newMesh;
    globalContext.testTriangle->m_transform = TransformSRT(Vec3(2, 1, 1), Quaternion((45.0f * 3.14f) / 180.0f, Vec3(0, 0, 1)), Vec3(0, 3, -5));

    // create the material
    Material * material = new Material();
    CreateTestMaterial(globalContext.pDevice, material);
    globalContext.material = material;

    MaterialInstance * inst = new MaterialInstance(material);
    globalContext.matInst = inst;

    // create the render state for the backbuffer
    CreateToBackBufferRenderState(globalContext.pDevice, &globalContext.toBackbufferRS, &globalContext.toBackbufferDSS, &globalContext.toBackBufferPassCb);

    // set up the camera
    globalContext.camera = new Camera();
    globalContext.camera->m_AspectRatio = 16.0f / 9.0f;
    globalContext.camera->m_near = 1.0f;
    globalContext.camera->m_far = 10.0f;
    globalContext.camera->m_wFov = (90.0f * 3.14f) / 180.0f;
    globalContext.camera->m_transform.translation = Vec3(0, 2, 0);
    globalContext.camera->RefreshProjMatrix();

    globalContext.pImmediateContext->QueryInterface(&globalContext.annotator);
    return true;
  }
  
  void Render() {
    // update the mesh uniform data 
    Matrix4 mymatrix = globalContext.testTriangle->m_transform.CalcMatrix();
    globalContext.matInst->SetParameter("gTransform", mymatrix);

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
    viewport.Width = 1600;
    viewport.Height = 900;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    globalContext.pImmediateContext->RSSetViewports(1, &viewport);
    
    D3D11_RECT rect;
    rect.top = 0;
    rect.left = 0;
    rect.bottom = 900;
    rect.right = 1600;

    // pass data
    globalContext.pImmediateContext->RSSetState(globalContext.toBackbufferRS);
    globalContext.pImmediateContext->RSSetScissorRects(1, &rect);
    globalContext.pImmediateContext->OMSetDepthStencilState(globalContext.toBackbufferDSS, 0);
    globalContext.camera->RefreshProjView();

    //set up pass constant buffer
    D3D11_MAPPED_SUBRESOURCE passCb;
    globalContext.pImmediateContext->Map(globalContext.toBackBufferPassCb, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &passCb);
    memcpy(passCb.pData, globalContext.camera->m_viewProj.v, sizeof(Matrix4));
    globalContext.pImmediateContext->Unmap(globalContext.toBackBufferPassCb, 0);
    globalContext.pImmediateContext->VSSetConstantBuffers(1, 1, &globalContext.toBackBufferPassCb);


    // material data
    BindMaterial(globalContext.pImmediateContext, globalContext.material, MaterialTechniqueID::kColor);

    //set the mesh
    BindMesh(globalContext.pImmediateContext, *globalContext.testTriangle->m_theMesh);

    // update the constant buffer data
    MapMaterialInstanceData(globalContext.pImmediateContext, globalContext.material, globalContext.matInst, MaterialTechniqueID::kColor);

    globalContext.pImmediateContext->DrawIndexed(3, 0, 0);

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

    delete globalContext.camera;
    globalContext.camera = nullptr;

    delete globalContext.testTriangle;
    globalContext.testTriangle = nullptr;

    delete globalContext.material;
    globalContext.material = nullptr;

    delete globalContext.matInst;
    globalContext.matInst = nullptr;

    globalContext.toBackbufferRS->Release();
    globalContext.toBackbufferDSS->Release();
    globalContext.toBackBufferPassCb->Release();

    globalContext.annotator->Release();
  }
}