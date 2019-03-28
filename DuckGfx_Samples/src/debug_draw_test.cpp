#include "debug_draw_test.h"
#include "DuckGfx_utils.h"
#include "input.h"

void DebugDraw_Test::Init() {
  m_scene = duckGfx::GetScene();
 
  m_material = duckGfx::GenerateDebugCubeMaterial();

  m_camera = duckGfx::ICamera::Create();
  m_camera->SetPerspective((30.0f * 3.14f) / 180.0f, 16.0f / 9.0f, 0.1f, 100.0f);
  m_cameraTrans = TransformRT{ tag::Identity{} };
  m_cameraTrans.translation = Vec3(0, 0, 4);
  m_camera->SetTransform(m_cameraTrans);

  m_model2 = duckGfx::GenerateDebugCubeModel();
  m_matInst2 = duckGfx::IMaterialInstance::Create(m_material);
  m_model2->SetMaterialInstance(m_matInst2);

  m_matInst2->SetParameter("color", Vec4(1, 1, 1, 1));
  m_matInst2->SetParameter("roughness", 50.0f);

  m_modelTrans2 = TransformSRT(Vec3(10, 1, 10),
                               Quaternion(tag::Identity{}),
                               Vec3(0, -2, -5));

  m_scene->SetAmbientColor(Vec3(0.05f, 0.05f, 0.05f));
  m_scene->SetLightDir(Vec3(1, 1, 0));
  m_scene->SetlightColor(Vec3(0.280f, 0.31f, 0.33f));
}

void DebugDraw_Test::OnStart() {
  m_scene->AddModel(m_model2);
  m_scene->SetMainCamera(m_camera);
}

void DebugDraw_Test::Update(float dt) {
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
  offset = m_cameraTrans.rotation.RotateVec(Vec4(offset, 0)).xyz();

  float rotAngle = 0;
  if (input::IsPressed(Key::Q)) {
    rotAngle += 2 * (3.14) / 180.0f;
  }
  if (input::IsPressed(Key::E)) {
    rotAngle -= 2 * (3.14) / 180.0f;
  }

  Quaternion rotation(rotAngle, Vec3(0, 1, 0));


  m_cameraTrans.translation += offset * 2 * dt;
  m_cameraTrans.rotation = rotation * m_cameraTrans.rotation;
  m_camera->SetTransform(m_cameraTrans);

  m_model2->SetTransform(m_modelTrans2);
  m_time += 2 * dt;

  Vec3 point1 = Vec3(std::cos(m_time), std::sin(m_time), -5);
  Vec3 point2 = Vec3(std::sin(m_time), 0, -5 + std::cos(m_time));
  m_scene->DrawDebugLine(point1, point2, Vec3(1, 0, 0));


  Vec3 point3 = Vec3(2 + std::cos(2 * m_time), std::sin(m_time), -5);
  Vec3 point4 = Vec3(2 + std::sin(2 * m_time), 0, -5 + std::cos(m_time));
  m_scene->DrawDebugLine(point3, point4, Vec3(0, 1, 0));

  Vec3 point5 = Vec3(-3 + std::cos(m_time), std::sin(m_time), -5);
  Vec3 point6 = Vec3(-3 + std::cos(m_time + 3.14f), std::sin(m_time + 3.14f), -5);
  m_scene->DrawDebugLine(point5, point6, Vec3(0, 0, 1));

  m_scene->DrawDebugAABB(Vec3(3, -1, -7), Vec3(4, 0, -8), Vec3(1, 1, 0));

  m_scene->DrawDebugSphere(Vec3(-3, -1.25, -4), 0.25f, Vec3(0, 1, 1));

  m_scene->DrawDebugOBB(TransformSRT{ Vec3(2, 1,1), Quaternion(1.0f, Vec3(1, 1,0)), Vec3(-3, -0.5, -8)}, Vec3(1, 0, 1));

  m_scene->DrawDebugCone(Vec3(0, 0, -7), 15.0f * (3.14f / 180.0f), 1.0f, Vec3(1, -1, -1), Vec3(1, 1, 1));
  m_scene->DrawDebugCone(Vec3(1, 0, -7), 15.0f * (3.14f / 180.0f), 2.0f, Vec3(1, -1, -1), Vec3(1, 1, 1));
}

void DebugDraw_Test::OnEnd() {
  m_scene->RemoveModel(m_model2);
  m_scene->SetMainCamera(nullptr);
}

void DebugDraw_Test::Shutdown() {
  duckGfx::IModel::Destroy(m_model2);
  m_model2 = nullptr;
  duckGfx::IMaterialInstance::Destroy(m_matInst2);
  m_matInst2 = nullptr;

  duckGfx::IMaterial::Destroy(m_material);
  m_material = nullptr;

  duckGfx::ICamera::Destroy(m_camera);
}