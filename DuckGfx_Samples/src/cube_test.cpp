#include "cube_test.h"
#include "DuckGfx_utils.h"

void Cube_Test::Init() {
  m_scene = duckGfx::GetScene();
  m_triangle = duckGfx::GenerateDebugCubeModel();

  m_modelTrans = TransformSRT(Vec3(1, 1, 1),
                 Quaternion((0 * 3.14f) / 180.0f, Vec3(1, 1, 0)),
                 Vec3(2, 0, -5));

  m_material = duckGfx::GenerateDebugCubeMaterial();

  m_matInst = duckGfx::IMaterialInstance::Create(m_material);
  m_matInst->SetParameter("color", Vec4(1, 0, 0, 1));
  m_matInst->SetParameter("roughness", 2.0f);

  m_triangle->SetMaterialInstance(m_matInst);

  m_camera = duckGfx::ICamera::Create();
  m_camera->SetPerspective((60.0f * 3.14f) / 180.0f, 16.0f / 9.0f, 1.0f, 100.0f);
  m_camera->SetTransform(TransformRT{ Quaternion{tag::Identity{}}, Vec3(0, 0, 0) });


  m_model2 = duckGfx::GenerateDebugCubeModel();
  m_matInst2 = duckGfx::IMaterialInstance::Create(m_material);
  m_model2->SetMaterialInstance(m_matInst2);

  m_matInst2->SetParameter("color", Vec4(0, 0, 1, 1));
  m_matInst2->SetParameter("roughness", 100.0f);

  m_modelTrans2 = TransformSRT(Vec3(1, 1, 1),
                               Quaternion((0 * 3.14f) / 180.0f, Vec3(1, 1, 0)),
                               Vec3(-2, 0, -5));



  m_angle = 0;
}

void Cube_Test::OnStart() {
  m_scene->AddModel(m_triangle);
  m_scene->AddModel(m_model2);
  m_scene->SetMainCamera(m_camera);
}

void Cube_Test::Update(float dt) {
  // rotate the triangle
  m_modelTrans.rotation = Quaternion(m_angle * (3.14f / 180.0f), Vec3(1, 1, 0));
  m_modelTrans2.rotation = Quaternion(m_angle * (3.14f / 180.0f), Vec3(.2, 1, 0));
  m_angle += 90.0f * dt;
  m_triangle->SetTransform(m_modelTrans);
  m_model2->SetTransform(m_modelTrans2);
}

void Cube_Test::OnEnd() {
  m_scene->RemoveModel(m_triangle);
  m_scene->RemoveModel(m_model2);
  m_scene->SetMainCamera(nullptr);
}

void Cube_Test::Shutdown() {
  duckGfx::IModel::Destroy(m_triangle);
  m_triangle = nullptr;
  duckGfx::IMaterialInstance::Destroy(m_matInst);
  m_matInst = nullptr;

  duckGfx::IModel::Destroy(m_model2);
  m_model2 = nullptr;
  duckGfx::IMaterialInstance::Destroy(m_matInst2);
  m_matInst2 = nullptr;

  duckGfx::IMaterial::Destroy(m_material);
  m_material = nullptr;
}