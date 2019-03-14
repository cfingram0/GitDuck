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

  void Scene::SetMainCamera(ICamera * camera) {
    m_mainCamera = reinterpret_cast<Camera *>(camera);
  }
}