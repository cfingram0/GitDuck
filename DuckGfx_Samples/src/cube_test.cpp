#include "cube_test.h"
#include "DuckGfx_utils.h"
#include "input.h"

void Cube_Test::Init() {
  m_scene = duckGfx::GetScene();
  m_triangle = duckGfx::GenerateDebugCubeModel();

  m_modelTrans = TransformSRT(Vec3(1, 1, 1),
                 Quaternion((0 * 3.14f) / 180.0f, Vec3(1, 1, 0)),
                 Vec3(2, 0, -5));

  m_material = duckGfx::GenerateDebugCubeMaterial();

  m_matInst = duckGfx::IMaterialInstance::Create(m_material);
  m_matInst->SetParameter("color", Vec4(1, 0, 0, 1));
  m_matInst->SetParameter("roughness", 1.0f);

  m_triangle->SetMaterialInstance(m_matInst);

  m_camera = duckGfx::ICamera::Create();
  m_camera->SetPerspective((30.0f * 3.14f) / 180.0f, 16.0f / 9.0f, 0.1f, 100.0f);
  m_cameraTrans = TransformRT{ tag::Identity{} };
  m_cameraTrans.translation = Vec3(0, 0, 4);
  m_camera->SetTransform(m_cameraTrans);


  m_model2 = duckGfx::GenerateDebugCubeModel();
  m_matInst2 = duckGfx::IMaterialInstance::Create(m_material);
  m_model2->SetMaterialInstance(m_matInst2);

  m_matInst2->SetParameter("color", Vec4(0, 0, 1, 1));
  m_matInst2->SetParameter("roughness", 0.2f);

  m_modelTrans2 = TransformSRT(Vec3(1, 1, 1),
                               Quaternion((0 * 3.14f) / 180.0f, Vec3(1, 1, 0)),
                               Vec3(-2, 0, -5));

  m_scene->SetAmbientColor(Vec3(0.2f, 0.2f, 0.2f));
  m_scene->SetLightDir(Vec3(1, 1, 1));
  m_scene->SetlightColor(Vec3(1.0f, 1.0f, 1.0f));

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
  m_modelTrans2.rotation = Quaternion(m_angle * (3.14f / 180.0f), Vec3(.2f, 1, 0));
  m_angle += 90.0f * dt;

  Vec3 offset{ tag::Zero{} };
  if (input::IsPressed(Key::W)) {
    offset += Vec3(0, 0, -1);
  }
  if (input::IsPressed(Key::S)) {
    offset += Vec3(0, 0, 1);
  }
  if (input::IsPressed(Key::A)) {
    offset += Vec3(-1, 0, 0);
  }
  if (input::IsPressed(Key::D)) {
    offset += Vec3(1, 0, 0);
  }
  if (input::IsPressed(Key::SPACE)) {
    offset += Vec3(0, 1, 0);
  }
  if (input::IsPressed(Key::C)) {
    offset += Vec3(0, -1, 0);
  }

  m_cameraTrans.translation += offset * 2 * dt;
  m_camera->SetTransform(m_cameraTrans);


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