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
}