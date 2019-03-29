#include "DuckGfx_internal.h"
namespace duckGfx {
  static bool GenerateDebugTriangle(Mesh* output) {
    output->format.hasPosition = 1;
    output->format.numColorSets = 1;

    // generate buffer for hello world triangle
    struct {
      Vec3 pos;
      Vec4 color;
    } vertData[] = { Vec3(-0.5f, 0.0f, 0.0f), Vec4(1.0f, 0.0f, 0.0f, 1),
                      Vec3(0.5f, 0.0f,  0.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f),
                      Vec3(0.5f, 0.5f,  0.0f), Vec4(0.0f, 0.0f, 1.0f, 1.0f) };

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

  IModel * GenerateDebugTriangleModel() {
    Mesh * newMesh = new Mesh();
    GenerateDebugTriangle(newMesh);

    Model * newModel = new Model();
    newModel->m_theMesh = newMesh;

    return newModel;
  }

  static bool GenerateDebugCube(Mesh * output) {
    output->format.hasPosition = 1;
    output->format.hasNormals = 1;

    // generate buffer for hello world triangle
    struct {
      Vec3 pos;
      Vec3 normal;  // side 1
    } vertData[] = {Vec3(-0.5, -0.5,  0.5), Vec3(0, 0, 1), // triangle 1
                    Vec3( 0.5, -0.5,  0.5), Vec3(0, 0, 1), // triangle 1
                    Vec3( 0.5,  0.5,  0.5), Vec3(0, 0, 1), // triangle 1

                    Vec3(-0.5, -0.5,  0.5), Vec3(0, 0, 1), // triangle 2
                    Vec3( 0.5,  0.5,  0.5), Vec3(0, 0, 1), // triangle 2
                    Vec3(-0.5,  0.5,  0.5), Vec3(0, 0, 1), // triangle 2
        
                    // side 2
                    Vec3( 0.5, -0.5,  0.5), Vec3(1, 0, 0), // triangle 3
                    Vec3( 0.5, -0.5,  -0.5), Vec3(1, 0, 0), // triangle 3
                    Vec3( 0.5,  0.5,  -0.5), Vec3(1, 0, 0), // triangle 3
                    
                    Vec3( 0.5, -0.5,  0.5), Vec3(1, 0, 0), // triangle 4
                    Vec3( 0.5,  0.5, -0.5), Vec3(1, 0, 0), // triangle 4
                    Vec3( 0.5,  0.5,  0.5), Vec3(1, 0, 0), // triangle 4

                    // side 3
                    Vec3( 0.5, -0.5, -0.5), Vec3(0, 0, -1), // triangle 5
                    Vec3(-0.5, -0.5, -0.5), Vec3(0, 0, -1), // triangle 5
                    Vec3(-0.5,  0.5, -0.5), Vec3(0, 0, -1), // triangle 5
                    
                    Vec3( 0.5, -0.5, -0.5), Vec3(0, 0, -1), // triangle 6
                    Vec3(-0.5,  0.5, -0.5), Vec3(0, 0, -1), // triangle 6
                    Vec3( 0.5,  0.5, -0.5), Vec3(0, 0, -1), // triangle 6
                         
                    // side 4
                    Vec3(-0.5, -0.5, -0.5), Vec3(-1, 0, 0), // triangle 7
                    Vec3(-0.5, -0.5,  0.5), Vec3(-1, 0, 0), // triangle 7
                    Vec3(-0.5,  0.5,  0.5), Vec3(-1, 0, 0), // triangle 7
                    
                    Vec3(-0.5, -0.5, -0.5), Vec3(-1, 0, 0), // triangle 8
                    Vec3(-0.5,  0.5,  0.5), Vec3(-1, 0, 0), // triangle 8
                    Vec3(-0.5,  0.5, -0.5), Vec3(-1, 0, 0), // triangle 8

                    // side 5
                    Vec3(-0.5,  0.5,  0.5), Vec3(0, 1, 0), // triangle 9
                    Vec3( 0.5,  0.5,  0.5), Vec3(0, 1, 0), // triangle 9
                    Vec3( 0.5,  0.5, -0.5), Vec3(0, 1, 0), // triangle 9
                    
                    Vec3(-0.5,  0.5,  0.5), Vec3(0, 1, 0), // triangle 10
                    Vec3( 0.5,  0.5, -0.5), Vec3(0, 1, 0), // triangle 10
                    Vec3(-0.5,  0.5, -0.5), Vec3(0, 1, 0), // triangle 10

                    // side 6
                    Vec3(-0.5, -0.5,  0.5), Vec3(0, -1, 0), // triangle 11
                    Vec3(-0.5, -0.5, -0.5), Vec3(0, -1, 0), // triangle 11
                    Vec3( 0.5, -0.5, -0.5), Vec3(0, -1, 0), // triangle 11
                    
                    Vec3(-0.5, -0.5,  0.5), Vec3(0, -1, 0), // triangle 12
                    Vec3( 0.5, -0.5, -0.5), Vec3(0, -1, 0), // triangle 12
                    Vec3( 0.5, -0.5,  0.5), Vec3(0, -1, 0), // triangle 12
                  };


    uint32_t numTris = 12;
    const uint32_t numVerts = 3 * numTris;
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

    uint32_t indices[] = { 0, 1, 2,
                           3, 4, 5,
                           6, 7, 8,
                           9, 10, 11,
                           12, 13, 14,
                           15, 16, 17,
                           18, 19, 20,
                           21, 22, 23,
                           24, 25, 26,
                           27, 28, 29,
                           30, 31, 32,
                           33, 34, 35
                           };

    D3D11_BUFFER_DESC desc_idx;
    desc_idx.Usage = D3D11_USAGE_DEFAULT;
    desc_idx.ByteWidth = sizeof(uint32_t) * 3 * numTris;
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
    output->idxCount = 36;
    output->topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    return true;
  }


  IModel * GenerateDebugCubeModel() {
    Mesh * newMesh = new Mesh();
    GenerateDebugCube(newMesh);

    Model * newModel = new Model();
    newModel->m_theMesh = newMesh;

    return newModel;
  }

  bool CreateTestMaterial(ID3D11Device * device, Material * outMaterial) {
    outMaterial->meshVertFormat.hasPosition = 1;
    outMaterial->meshVertFormat.numColorSets = 1;

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
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vertShader = vertShader;

    ID3D11InputLayout * pLayout;
    CreateVertLayout(device, outMaterial->meshVertFormat, bytes.data(), bytes.size(), &pLayout);
    outMaterial->inputLayout = pLayout;

    ID3D11PixelShader * pixelShader;
    success = ReadAllBytes("TestPixelShader.cso", &bytes);
    if (!success) {
      return false;
    }
    globalContext.pDevice->CreatePixelShader(bytes.data(), bytes.size(), nullptr, &pixelShader);
    if (!pixelShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_pixelShader = pixelShader;


    ID3D11Buffer * constantBuffer = NULL;
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(Matrix4);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    HRESULT hr = globalContext.pDevice->CreateBuffer(&cbDesc, NULL, &constantBuffer);
    if (FAILED(hr)) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBuffer = constantBuffer;
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBufferSize = sizeof(float) * 16;
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBufferSlot = 0;


    Material::VariableMap map;
    map.dataStartVs[MaterialTechniqueID::kColor] = 0;
    map.elementType = MaterialParameterType::kFloat;
    map.numValues = 16;
    map.varName = "gTransform";

    outMaterial->variables.push_back(map);

    return true;
  }

  IMaterial * GenerateDebugMaterial() {
    Material * mat = new Material();
    if (!CreateTestMaterial(globalContext.pDevice, mat)) {
      delete mat;
      return nullptr;
    }

    return mat;
  }

  bool CreateTestCubeMaterial(ID3D11Device * device, Material * outMaterial) {
    outMaterial->meshVertFormat.hasPosition = 1;
    outMaterial->meshVertFormat.hasNormals = 1;

    // load the shaders
    std::vector<uint8_t> bytes;
    bool success = ReadAllBytes("TestCubeVertexShader.cso", &bytes);
    if (!success) {
      return false;
    }

    ID3D11VertexShader * vertShader = nullptr;
    globalContext.pDevice->CreateVertexShader(bytes.data(), bytes.size(), nullptr, &vertShader);
    if (!vertShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vertShader = vertShader;

    ID3D11InputLayout * pLayout;
    CreateVertLayout(device, outMaterial->meshVertFormat, bytes.data(), bytes.size(), &pLayout);
    outMaterial->inputLayout = pLayout;

    ID3D11PixelShader * pixelShader;
    success = ReadAllBytes("TestCubePixelShader.cso", &bytes);
    if (!success) {
      return false;
    }
    globalContext.pDevice->CreatePixelShader(bytes.data(), bytes.size(), nullptr, &pixelShader);
    if (!pixelShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_pixelShader = pixelShader;


    ID3D11Buffer * constantBuffer = NULL;
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = 2 * sizeof(Matrix4);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    HRESULT hr = globalContext.pDevice->CreateBuffer(&cbDesc, NULL, &constantBuffer);
    if (FAILED(hr)) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBuffer = constantBuffer;
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBufferSize = 2 * sizeof(Matrix4);
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBufferSlot = 0;


    Material::VariableMap map;
    map.dataStartVs[MaterialTechniqueID::kColor] = 0;
    map.elementType = MaterialParameterType::kFloat;
    map.numValues = 16;
    map.varName = "gTransform";

    outMaterial->variables.push_back(map);

    Material::VariableMap map2;
    map2.dataStartVs[MaterialTechniqueID::kColor] = sizeof(Matrix4);
    map2.elementType = MaterialParameterType::kFloat;
    map2.numValues = 16;
    map2.varName = "gNormalTransform";
    outMaterial->variables.push_back(map2);

    //pixel constant buffer
    ID3D11Buffer * constantBuffer2 = NULL;
    D3D11_BUFFER_DESC cbDesc2;
    cbDesc2.ByteWidth = 2 * sizeof(Vec4);
    cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc2.MiscFlags = 0;
    cbDesc2.StructureByteStride = 0;

    hr = globalContext.pDevice->CreateBuffer(&cbDesc2, NULL, &constantBuffer2);
    if (FAILED(hr)) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_psConstantBuffer = constantBuffer2;
    outMaterial->techniques[MaterialTechniqueID::kColor].m_psConstantBufferSize = 2 * sizeof(Vec4);
    outMaterial->techniques[MaterialTechniqueID::kColor].m_psConstantBufferSlot = 0;


    Material::VariableMap map3;
    map3.dataStartPs[MaterialTechniqueID::kColor] = 0;
    map3.elementType = MaterialParameterType::kFloat;
    map3.numValues = 4;
    map3.varName = "color";
    outMaterial->variables.push_back(map3);

    Material::VariableMap map4;
    map4.dataStartPs[MaterialTechniqueID::kColor] = sizeof(Vec4);
    map4.elementType = MaterialParameterType::kFloat;
    map4.numValues = 1;
    map4.varName = "roughness";

    outMaterial->variables.push_back(map4);

    return true;
  }

  IMaterial * GenerateDebugCubeMaterial() {
    Material * mat = new Material();
    if (!CreateTestCubeMaterial(globalContext.pDevice, mat)) {
      delete mat;
      return nullptr;
    }

    return mat;
  }


  bool CreateDebugDrawMaterial(ID3D11Device * device, Material * outMaterial) {
    outMaterial->meshVertFormat.hasPosition = 1;
    outMaterial->meshVertFormat.hasNormals = 1;

    // load the shaders
    std::vector<uint8_t> bytes;
    bool success = ReadAllBytes("DebugVertexShader.cso", &bytes);
    if (!success) {
      return false;
    }

    ID3D11VertexShader * vertShader = nullptr;
    globalContext.pDevice->CreateVertexShader(bytes.data(), bytes.size(), nullptr, &vertShader);
    if (!vertShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vertShader = vertShader;

    ID3D11InputLayout * pLayout;
    CreateVertLayout(device, outMaterial->meshVertFormat, bytes.data(), bytes.size(), &pLayout);
    outMaterial->inputLayout = pLayout;

    ID3D11PixelShader * pixelShader;
    success = ReadAllBytes("DebugPixelShader.cso", &bytes);
    if (!success) {
      return false;
    }
    globalContext.pDevice->CreatePixelShader(bytes.data(), bytes.size(), nullptr, &pixelShader);
    if (!pixelShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_pixelShader = pixelShader;


    ID3D11Buffer * constantBuffer = NULL;
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = 2 * sizeof(Matrix4);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    HRESULT hr = globalContext.pDevice->CreateBuffer(&cbDesc, NULL, &constantBuffer);
    if (FAILED(hr)) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBuffer = constantBuffer;
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBufferSize = 2 * sizeof(Matrix4);
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vsConstantBufferSlot = 0;


    Material::VariableMap map;
    map.dataStartVs[MaterialTechniqueID::kColor] = 0;
    map.elementType = MaterialParameterType::kFloat;
    map.numValues = 16;
    map.varName = "gTransform";

    outMaterial->variables.push_back(map);

    Material::VariableMap map2;
    map2.dataStartVs[MaterialTechniqueID::kColor] = sizeof(Matrix4);
    map2.elementType = MaterialParameterType::kFloat;
    map2.numValues = 16;
    map2.varName = "gNormalTransform";
    outMaterial->variables.push_back(map2);

    //pixel constant buffer
    ID3D11Buffer * constantBuffer2 = NULL;
    D3D11_BUFFER_DESC cbDesc2;
    cbDesc2.ByteWidth = sizeof(Vec4);
    cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc2.MiscFlags = 0;
    cbDesc2.StructureByteStride = 0;

    hr = globalContext.pDevice->CreateBuffer(&cbDesc2, NULL, &constantBuffer2);
    if (FAILED(hr)) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_psConstantBuffer = constantBuffer2;
    outMaterial->techniques[MaterialTechniqueID::kColor].m_psConstantBufferSize = sizeof(Vec4);
    outMaterial->techniques[MaterialTechniqueID::kColor].m_psConstantBufferSlot = 0;


    Material::VariableMap map3;
    map3.dataStartPs[MaterialTechniqueID::kColor] = 0;
    map3.elementType = MaterialParameterType::kFloat;
    map3.numValues = 4;
    map3.varName = "color";
    outMaterial->variables.push_back(map3);

    return true;
  }


  IMaterial * GenerateDebugDrawMaterial() {
    Material * mat = new Material();
    if (!CreateDebugDrawMaterial(globalContext.pDevice, mat)) {
      delete mat;
      return nullptr;
    }

    return mat;
  }


  bool GenerateDebugLineMesh(Mesh * output) {
    output->format.hasPosition = 1;
    output->format.hasNormals = 1;

    // generate vertex buffer for line
    struct {
      Vec3 pos;
      Vec3 normal;
    } vertData[] = { Vec3(0.0f, 0.0f, 0.0f), Vec3(0,0,0), Vec3(1.0f, 0.0f,  0.0f), Vec3(0,0,0)};

    const uint32_t numVerts = 2;
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

    uint32_t indices[] = { 0, 1 };
    D3D11_BUFFER_DESC desc_idx;
    desc_idx.Usage = D3D11_USAGE_DEFAULT;
    desc_idx.ByteWidth = sizeof(uint32_t) * 2;
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
    output->idxCount = 2;
    output->topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    return true;
  }


  bool GenerateQuad(ID3D11Device * device, Mesh * output) {
    output->format.hasPosition = 1;
    output->format.numUVsets = 1;

    // generate vertex buffer for line
    struct {
      Vec3 pos;
      Vec2 texCoord;
    } vertData[] = { Vec3(-1.0f, -1.0f, 0.0f), Vec2(0,1), 
                     Vec3( 1.0f, -1.0f, 0.0f), Vec2(1,1), 
                     Vec3( 1.0f,  1.0f, 0.0f), Vec2(1,0), 
                     Vec3(-1.0f,  1.0f, 0.0f), Vec2(0,0)};

    const uint32_t numVerts = 4;
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
    HRESULT hr = device->CreateBuffer(&desc, &initData, &vertBuffer);
    if (FAILED(hr)) {
      return false;
    }
    output->vertBuffer = vertBuffer;

    uint32_t indices[] = { 0, 1, 3, 1, 2, 3};
    D3D11_BUFFER_DESC desc_idx;
    desc_idx.Usage = D3D11_USAGE_DEFAULT;
    desc_idx.ByteWidth = sizeof(uint32_t) * 6;
    desc_idx.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc_idx.CPUAccessFlags = 0;
    desc_idx.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData_idx;
    initData_idx.pSysMem = indices;
    initData_idx.SysMemPitch = 0;
    initData_idx.SysMemSlicePitch = 0;

    ID3D11Buffer * idxBuffer;
    hr = device->CreateBuffer(&desc_idx, &initData_idx, &idxBuffer);
    if (FAILED(hr)) {
      return false;
    }

    output->idxBuffer = idxBuffer;
    output->idxCount = 6;
    output->topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    return true;
  }

  bool CreateToBackbufferMaterial(ID3D11Device * device, Material * outMaterial) {
    outMaterial->meshVertFormat.hasPosition = 1;
    outMaterial->meshVertFormat.numUVsets = 1;

    // load the shaders
    std::vector<uint8_t> bytes;
    bool success = ReadAllBytes("ToBackBufferVertexShader.cso", &bytes);
    if (!success) {
      return false;
    }

    ID3D11VertexShader * vertShader = nullptr;
    globalContext.pDevice->CreateVertexShader(bytes.data(), bytes.size(), nullptr, &vertShader);
    if (!vertShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_vertShader = vertShader;

    ID3D11InputLayout * pLayout;
    CreateVertLayout(device, outMaterial->meshVertFormat, bytes.data(), bytes.size(), &pLayout);
    outMaterial->inputLayout = pLayout;

    ID3D11PixelShader * pixelShader;
    success = ReadAllBytes("ToBackBufferPixelShader.cso", &bytes);
    if (!success) {
      return false;
    }
    globalContext.pDevice->CreatePixelShader(bytes.data(), bytes.size(), nullptr, &pixelShader);
    if (!pixelShader) {
      return false;
    }
    outMaterial->techniques[MaterialTechniqueID::kColor].m_pixelShader = pixelShader;
    return true;
  }

}