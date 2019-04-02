#include "DuckGfx_internal.h"

namespace duckGfx {

  void BindMaterial(ID3D11DeviceContext * context, Material * mat, MaterialTechniqueID technique) {
    MaterialTechnique & tech = mat->techniques[technique];
    context->VSSetShader(tech.m_vertShader, nullptr, 0);
    if (tech.m_vsConstantBuffer) {
      context->VSSetConstantBuffers(tech.m_vsConstantBufferSlot, 1, &tech.m_vsConstantBuffer);
    }

    context->PSSetShader(tech.m_pixelShader, nullptr, 0);
    if (tech.m_psConstantBuffer) {
      context->PSSetConstantBuffers(tech.m_psConstantBufferSlot, 1, &tech.m_psConstantBuffer);
    }

    context->IASetInputLayout(mat->inputLayout);
  }

  void MapMaterialInstanceData(ID3D11DeviceContext * context, Material * mat, MaterialInstance * inst, MaterialTechniqueID technique) {
    MaterialTechnique & tech = mat->techniques[technique];
    
    if (tech.m_vsConstantBuffer) {
      D3D11_MAPPED_SUBRESOURCE subresource;
      ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
      context->Map(tech.m_vsConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);

      std::vector<uint8_t> & data = inst->m_cpuVsConstantBuffer[technique];
      assert(data.size() == tech.m_vsConstantBufferSize);
      memcpy(subresource.pData, data.data(), data.size());
      context->Unmap(tech.m_vsConstantBuffer, 0);
    }

    if (tech.m_psConstantBuffer) {
      D3D11_MAPPED_SUBRESOURCE subresource;
      ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
      context->Map(tech.m_psConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);

      std::vector<uint8_t> & data = inst->m_cpuPsConstantBuffer[technique];
      assert(data.size() == tech.m_psConstantBufferSize);
      memcpy(subresource.pData, data.data(), data.size());
      context->Unmap(tech.m_psConstantBuffer, 0);
    }
  }

  bool CreateVertLayout(ID3D11Device * device, const VertexFormat & fmt, void * shaderByteCode, size_t shaderbyteCodeLength, ID3D11InputLayout ** outLayout) {
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

    if (fmt.hasPosition) {
      D3D11_INPUT_ELEMENT_DESC posLayout = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
      layout.push_back(posLayout);
    }
    if (fmt.hasNormals) {
      D3D11_INPUT_ELEMENT_DESC normalLayout = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
      layout.push_back(normalLayout);
    }
    if (fmt.hasTangentFrame) {
      D3D11_INPUT_ELEMENT_DESC tanFrameLayout[] = { { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
      layout.push_back(tanFrameLayout[0]);
      layout.push_back(tanFrameLayout[1]);
    }
    if (fmt.hasBlendWeightsAndIndices) {
      // need to thank about this one, just assert fail for now
      assert(false); // not implemented
    }
    for (uint32_t i = 0; i < fmt.numUVsets; ++i) {
      D3D11_INPUT_ELEMENT_DESC texLayout = { "TEXCOORD", i, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
      layout.push_back(texLayout);
    }
    for (uint32_t i = 0; i < fmt.numColorSets; ++i) {
      D3D11_INPUT_ELEMENT_DESC colorLayout = { "COLOR", i, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
      layout.push_back(colorLayout);
    }

    device->CreateInputLayout(&layout[0], UINT(layout.size()), shaderByteCode, shaderbyteCodeLength, outLayout);
    return true;
  }


  void IMaterial::Destroy(IMaterial * mat) {
    delete reinterpret_cast<Material*>(mat);
  }

  MaterialInstance::MaterialInstance(Material * material)
  :m_material(material){
    for (int32_t i = 0; i < MaterialTechniqueID::kCount; ++i) {
      m_cpuPsConstantBuffer[i].resize(material->techniques[i].m_psConstantBufferSize);
      m_cpuVsConstantBuffer[i].resize(material->techniques[i].m_vsConstantBufferSize);
    }
  }

  void MaterialInstance::WriteToBuffers(const Material::VariableMap * varInfo, void * data, uint32_t size) {
    for (int32_t i = 0; i < MaterialTechniqueID::kCount; ++i) {
      int32_t startPsIdx = varInfo->dataStartPs[i];
      if (startPsIdx >= 0) {
        assert(m_cpuPsConstantBuffer[i].size() >= startPsIdx + size);
        std::vector<uint8_t> & cb = m_cpuPsConstantBuffer[i];
        memcpy(&cb[startPsIdx], data, size);
      }

      int32_t startVsIdx = varInfo->dataStartVs[i];
      if (startVsIdx >= 0) {
        assert(m_cpuVsConstantBuffer[i].size() >= startVsIdx + size);
        std::vector<uint8_t> & cb = m_cpuVsConstantBuffer[i];
        memcpy(&cb[startVsIdx], data, size);
      }
    }
  }

  bool MaterialInstance::SetParameter(StringHash name, float * data, uint32_t count) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo 
        && varInfo->elementType == MaterialParameterType::kFloat 
        && varInfo->numValues == count) {
      WriteToBuffers(varInfo, data, sizeof(float) * count);
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, uint32_t * data, uint32_t count) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kUint
      && varInfo->numValues == count) {
      WriteToBuffers(varInfo, data, sizeof(uint32_t) * count);
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, int32_t * data, uint32_t count) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kInt
      && varInfo->numValues == count) {
      WriteToBuffers(varInfo, data, sizeof(int32_t) * count);
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, float data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kFloat
      && varInfo->numValues == 1) {
      WriteToBuffers(varInfo, &data, sizeof(float));
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, uint32_t data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kUint
      && varInfo->numValues == 1) {
      WriteToBuffers(varInfo, &data, sizeof(uint32_t));
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, int32_t data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kInt
      && varInfo->numValues == 1) {
      WriteToBuffers(varInfo, &data, sizeof(int32_t));
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, const Vec2 & data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);;
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kFloat
      && varInfo->numValues == 2) {
      WriteToBuffers(varInfo, (void*)data.m, sizeof(float) * 2);
      return true;
    }

    return false;
  }
  bool MaterialInstance::SetParameter(StringHash name, const Vec3 & data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kFloat
      && varInfo->numValues == 3) {
      WriteToBuffers(varInfo, (void*)data.m, sizeof(float) * 3);
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, const Vec4 & data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kFloat
      && varInfo->numValues == 4) {
      WriteToBuffers(varInfo, (void*)data.m, sizeof(float) * 4);
      return true;
    }

    return false;
  }

  bool MaterialInstance::SetParameter(StringHash name, const Matrix4 & data) {
    const Material::VariableMap * varInfo = m_material->FindVariableData(name);
    if (varInfo
      && varInfo->elementType == MaterialParameterType::kFloat
      && varInfo->numValues == 16) {
      WriteToBuffers(varInfo, (void*)data.v, sizeof(float) * 16);
      return true;
    }

    return false;
  }

  const Material::VariableMap * Material::FindVariableData(StringHash name) const {
    for (uint32_t i = 0; i < variables.size(); ++i) {
      if (name == variables[i].varName) {
        return &variables[i];
      }
    }
    return nullptr;
  }

  Material::~Material() {
    if(inputLayout)
      inputLayout->Release();

    for (int32_t i = 0; i < MaterialTechniqueID::kCount; ++i) {
      MaterialTechnique & tech = techniques[i];
      if(tech.m_pixelShader)
        tech.m_pixelShader->Release();
      
      if (tech.m_psConstantBuffer)
        tech.m_psConstantBuffer->Release();

      if(tech.m_vertShader)
        tech.m_vertShader->Release();
      
      if (tech.m_vsConstantBuffer)
        tech.m_vsConstantBuffer->Release();
    }
  }

  IMaterialInstance * IMaterialInstance::Create(IMaterial * mat) {
    MaterialInstance * inst = new MaterialInstance(reinterpret_cast<Material*>(mat));
    return inst;
  }
  void IMaterialInstance::Destroy(IMaterialInstance * inst) {
    MaterialInstance * instance = reinterpret_cast<MaterialInstance *>(inst);
    delete instance;
  }


}