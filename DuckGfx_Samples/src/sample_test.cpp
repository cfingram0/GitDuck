#include "sample_test.h"
#include "DuckGfx_utils.h"

void Sample_Test::Init() {
  m_scene = duckGfx::GetScene();
  m_triangle = duckGfx::GenerateDebugTriangleModel();

  m_modelTrans = TransformSRT(Vec3(2, 1, 1),
                 Quaternion((0 * 3.14f) / 180.0f, Vec3(0, 0, 1)),
                 Vec3(0, 3, -5));

  m_material = duckGfx::GenerateDebugMaterial();

  m_matInst = duckGfx::IMaterialInstance::Create(m_material);

  m_triangle->SetMaterialInstance(m_matInst);

  m_angle = 0;
}

void Sample_Test::OnStart() {
  m_scene->AddModel(m_triangle);
}

void Sample_Test::Update(float dt) {
  // rotate the triangle
  m_modelTrans.rotation = Quaternion(m_angle * (3.14f / 180.0f), Vec3(0, 0, 1));
  m_angle += 90.0f * dt;
  m_triangle->SetTransform(m_modelTrans);
}

void Sample_Test::OnEnd() {
  m_scene->RemoveModel(m_triangle);
}

void Sample_Test::Shutdown() {
  duckGfx::IModel::Destroy(m_triangle);
  m_triangle = nullptr;
  duckGfx::IMaterialInstance::Destroy(m_matInst);
  m_matInst = nullptr;

  duckGfx::IMaterial::Destroy(m_material);
  m_material = nullptr;

}