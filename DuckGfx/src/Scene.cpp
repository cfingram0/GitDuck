#include "DuckGfx_internal.h"
namespace duckGfx {

  IScene * GetScene() {
    return &globalContext.theScene;
  }

  void Scene::AddModel(IModel * model) {
    Model * modelInt = reinterpret_cast<Model *>(model);

    for (uint32_t i = 0; i < m_models.size(); ++i)
      assert(m_models[i] != modelInt);

    m_models.push_back(modelInt);
  }

  void Scene::RemoveModel(IModel * model) {
    Model * modelInt = reinterpret_cast<Model *>(model);

    for (uint32_t i = 0; i < m_models.size(); ++i) {
      if (m_models[i] == modelInt) {
        m_models[i] = m_models[m_models.size() - 1];
        m_models.resize(m_models.size() - 1);
        break;
      }
    }
  }

  void Scene::AddLight(IPointLight * light) {
    PointLight * lightInt = reinterpret_cast<PointLight *>(light);

    for (uint32_t i = 0; i < m_pointLights.size(); ++i)
      assert(m_pointLights[i] != lightInt);

    m_pointLights.push_back(lightInt);
  }

  void Scene::RemoveLight(IPointLight * light) {
    PointLight * lightInt = reinterpret_cast<PointLight *>(light);

    for (uint32_t i = 0; i < m_pointLights.size(); ++i) {
      if (m_pointLights[i] == lightInt) {
        m_pointLights[i] = m_pointLights[m_pointLights.size() - 1];
        m_pointLights.resize(m_pointLights.size() - 1);
        break;
      }
    }
  }

  void Scene::AddLight(ISpotLight * light) {
    SpotLight * lightInt = reinterpret_cast<SpotLight *>(light);

    for (uint32_t i = 0; i < m_spotLights.size(); ++i)
      assert(m_spotLights[i] != lightInt);

    m_spotLights.push_back(lightInt);
  }

  void Scene::RemoveLight(ISpotLight * light) {
    SpotLight * lightInt = reinterpret_cast<SpotLight *>(light);

    for (uint32_t i = 0; i < m_spotLights.size(); ++i) {
      if (m_spotLights[i] == lightInt) {
        m_spotLights[i] = m_spotLights[m_spotLights.size() - 1];
        m_spotLights.resize(m_spotLights.size() - 1);
        break;
      }
    }
  }

  void Scene::DrawDebugLine(const Vec3 & a, const Vec3 & b, const Vec3 & color) {
    m_debugLines.push_back({ a, b, color });
  }

  void Scene::DrawDebugSphere(const Vec3 & pos, float r, const Vec3 & color) {
    const float numDiv = 30;

    //one circle for each axis, then one orthogonal to the camera
    for (int i = 0; i < (int)numDiv; ++i) {
      Vec3 v = Vec3(r, 0, 0);
      Vec3 rotA = Quaternion(6.28f * (i / numDiv), Vec4(0, 0, 1, 0)).RotateVec(Vec4(v, 0)).xyz() + pos;
      Vec3 rotB = Quaternion(6.28f * ((i + 1)/ numDiv), Vec4(0, 0, 1, 0)).RotateVec(Vec4(v, 0)).xyz() + pos;
      DrawDebugLine(rotA, rotB, color);
    }

    for (int i = 0; i < (int)numDiv; ++i) {
      Vec3 v = Vec3(r, 0, 0);
      Vec3 rotA = Quaternion(6.28f * (i / numDiv), Vec4(0, 1, 0, 0)).RotateVec(Vec4(v, 0)).xyz() + pos;
      Vec3 rotB = Quaternion(6.28f * ((i + 1) / numDiv), Vec4(0, 1, 0, 0)).RotateVec(Vec4(v, 0)).xyz() + pos;
      DrawDebugLine(rotA, rotB, color);
    }

    for (int i = 0; i < (int)numDiv; ++i) {
      Vec3 v = Vec3(0, r, 0);
      Vec3 rotA = Quaternion(6.28f * (i / numDiv), Vec4(1, 0, 0, 0)).RotateVec(Vec4(v, 0)).xyz() + pos;
      Vec3 rotB = Quaternion(6.28f * ((i + 1) / numDiv), Vec4(1, 0, 0, 0)).RotateVec(Vec4(v, 0)).xyz() + pos;
      DrawDebugLine(rotA, rotB, color);
    }

    if (m_mainCamera) {
      for (int i = 0; i < (int)numDiv; ++i) {
        Vec4 axis = Vec4(m_mainCamera->m_transform.translation - pos, 0);
        axis = Normalize(axis);
      
        Vec3 v = Normalize(Vec3(-axis.y, axis.x, 0)) * r;
      
        Vec3 rotA = Quaternion(6.28f * (i / numDiv), axis).RotateVec(Vec4(v, 0)).xyz() + pos;
        Vec3 rotB = Quaternion(6.28f * ((i + 1) / numDiv), axis).RotateVec(Vec4(v, 0)).xyz() + pos;
        DrawDebugLine(rotA, rotB, color);
      }
    }

  }

  void Scene::DrawDebugAABB(const Vec3 & min, const Vec3 & max, const Vec3 & color) {
    Vec3 arr[8] = {
      Vec3(min.x, min.y, min.z),
      Vec3(max.x, min.y, min.z),
      Vec3(max.x, max.y, min.z),
      Vec3(min.x, max.y, min.z),
      Vec3(min.x, min.y, max.z),
      Vec3(max.x, min.y, max.z),
      Vec3(max.x, max.y, max.z),
      Vec3(min.x, max.y, max.z)
    };
    
    DrawDebugLine(arr[0], arr[1], color);
    DrawDebugLine(arr[1], arr[2], color);
    DrawDebugLine(arr[2], arr[3], color);
    DrawDebugLine(arr[3], arr[0], color);
    
    DrawDebugLine(arr[0 + 4], arr[1 + 4], color);
    DrawDebugLine(arr[1 + 4], arr[2 + 4], color);
    DrawDebugLine(arr[2 + 4], arr[3 + 4], color);
    DrawDebugLine(arr[3 + 4], arr[0 + 4], color);

    DrawDebugLine(arr[0], arr[0 + 4], color);
    DrawDebugLine(arr[1], arr[1 + 4], color);
    DrawDebugLine(arr[2], arr[2 + 4], color);
    DrawDebugLine(arr[3], arr[3 + 4], color);
  }

  void Scene::DrawDebugOBB(TransformSRT & transform, const Vec3 & color) {
    Vec3 arr[8] = {
      Vec3(-0.5f, -0.5f, -0.5f),
      Vec3(0.5f, -0.5f, -0.5f),
      Vec3(0.5f, 0.5f, -0.5f),
      Vec3(-0.5f, 0.5f, -0.5f),
      Vec3(-0.5f, -0.5f, 0.5f),
      Vec3(0.5f, -0.5f, 0.5f),
      Vec3(0.5f, 0.5f, 0.5f),
      Vec3(-0.5f, 0.5f, 0.5f)
    };

    for (int i = 0; i < 8; ++i) {
      arr[i] = (transform.CalcMatrix() * Vec4(arr[i], 1)).xyz();
    }

    DrawDebugLine(arr[0], arr[1], color);
    DrawDebugLine(arr[1], arr[2], color);
    DrawDebugLine(arr[2], arr[3], color);
    DrawDebugLine(arr[3], arr[0], color);

    DrawDebugLine(arr[0 + 4], arr[1 + 4], color);
    DrawDebugLine(arr[1 + 4], arr[2 + 4], color);
    DrawDebugLine(arr[2 + 4], arr[3 + 4], color);
    DrawDebugLine(arr[3 + 4], arr[0 + 4], color);

    DrawDebugLine(arr[0], arr[0 + 4], color);
    DrawDebugLine(arr[1], arr[1 + 4], color);
    DrawDebugLine(arr[2], arr[2 + 4], color);
    DrawDebugLine(arr[3], arr[3 + 4], color);
  }

  void Scene::DrawDebugCone(const Vec3 & tip, float halfAngle, float length, const Vec3 & direction, const Vec3 & color) {
    Vec3 dir = Normalize(direction);
    Vec3 baseCenter = tip + dir * length;

    constexpr int numDiv = 20;
    std::vector<Vec3> points(numDiv, tag::NoInit{});

    float oppLength = std::tan(halfAngle) * length;
    Vec3 arm = Normalize(Vec3(-direction.y, direction.x, 0)) * oppLength;

    for (int i = 0; i < numDiv; ++i) {
      points[i] = Quaternion(6.28f * (i / (float)numDiv), Vec4(dir, 0)).RotateVec(Vec4(arm, 0)).xyz() + baseCenter;
    }
    
    for (int i = 0; i < numDiv; ++i) {
      DrawDebugLine(points[i], points[(i + 1) % numDiv], color);
      if (i % 2 == 0) {
        DrawDebugLine(points[i], tip, color);
      }
    }

  }

  void Scene::SetMainCamera(ICamera * camera) {
    m_mainCamera = reinterpret_cast<Camera *>(camera);
  }

  void Scene::SetLightDir(const Vec3 & rhs) {
    m_lightDirection = Normalize(rhs);
  }

  void Scene::SetlightColor(const Vec3 & rhs) {
    m_lightColor = rhs;
  }

  void Scene::SetAmbientColor(const Vec3 & rhs) {
    m_ambientColor = rhs;
  }
}