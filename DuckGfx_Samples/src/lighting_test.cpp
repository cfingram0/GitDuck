#include "lighting_test.h"
#include "DuckGfx_utils.h"
#include "input.h"

void Lighting_Test::Init() {
  m_scene = duckGfx::GetScene();
  m_triangle = duckGfx::GenerateDebugCubeModel();

  m_modelTrans = TransformSRT(Vec3(1, 1, 1),
                 Quaternion((0 * 3.14f) / 180.0f, Vec3(1, 1, 0)),
                 Vec3(0, 0, -5));

  m_material = duckGfx::GenerateDebugCubeMaterial();

  m_matInst = duckGfx::IMaterialInstance::Create(m_material);
  m_matInst->SetParameter("color", Vec4(1, 0, 0, 1));
  m_matInst->SetParameter("roughness", 50.0f);

  m_triangle->SetMaterialInstance(m_matInst);

  m_camera = duckGfx::ICamera::Create();
  m_camera->SetPerspective((60.0f * 3.14f) / 180.0f, 16.0f / 9.0f, 0.1f, 100.0f);
  m_cameraTrans = TransformRT{ tag::Identity{} };
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
  m_scene->SetlightColor(Vec3(0.180f, 0.21f, 0.23f));

  m_pLight = duckGfx::IPointLight::Create();
  m_pLight->SetColor(Vec3(1.0f, 0.57f, 0.16f));
  m_pLight->SetIntensity(2.0f);
  m_pLight->SetPosition(Vec3(-2, 0, -5));


  m_pLight2 = duckGfx::IPointLight::Create();
  m_pLight2->SetColor(Vec3(0.16f, 0.57f, 1.0f));
  m_pLight2->SetIntensity(3.0f);
  m_pLight2->SetPosition(Vec3(-2, 0, -2));

  m_sLight = duckGfx::ISpotLight::Create();
  m_sLight->SetColor(Vec3(1.0f, 1.0f, 1.0f));
  m_sLight->SetPosition(Vec3(1.0f, 1.0f, -5.0f));
  m_sLight->SetDirection(Vec3(0, -1, 0));
  m_sLight->SetAngle(35.0f, 40.0f);
  m_sLight->SetIntensity(1.0f);

  m_sLight2 = duckGfx::ISpotLight::Create();
  m_sLight2->SetColor(Vec3(1.0f, 1.0f, 1.0f));
  m_sLight2->SetPosition(Vec3(1.0f, 1.0f, -5.0f));
  m_sLight2->SetDirection(Vec3(0, -1, 0));
  m_sLight2->SetAngle(35.0f, 40.0f);
  m_sLight2->SetIntensity(1.0f);

  m_angle = 0;
}

void Lighting_Test::OnStart() {
  m_scene->AddModel(m_triangle);
  m_scene->AddModel(m_model2);
  m_scene->SetMainCamera(m_camera);
  m_scene->AddLight(m_pLight);
  m_scene->AddLight(m_sLight);
  m_scene->AddLight(m_pLight2);
  m_scene->AddLight(m_sLight2);
}

void Lighting_Test::Update(float dt) {
  // rotate the triangle
  m_modelTrans.rotation = Quaternion(m_angle * (3.14f / 180.0f), Vec3(1, 1, .2f));
  m_angle += 40.0f * dt;

  Quaternion rotate(std::sin(2 * m_angle * 3.14f / 180.0f), Vec3(1, 0, 1));
  Vec4 newRot = rotate.RotateVec(Vec4(0, -1, 0, 0));
  m_sLight->SetDirection(newRot.xyz());

  Quaternion rotate2(std::sin(2 * m_angle * 3.14f / 180.0f), Vec3(1, 0, 0));
  newRot = rotate2.RotateVec(Vec4(0, -1, 0, 0));
  m_sLight2->SetDirection(newRot.xyz());

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

void Lighting_Test::OnEnd() {
  m_scene->RemoveModel(m_triangle);
  m_scene->RemoveModel(m_model2);
  m_scene->SetMainCamera(nullptr);
  m_scene->RemoveLight(m_pLight);
  m_scene->RemoveLight(m_sLight);
}

void Lighting_Test::Shutdown() {
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

  duckGfx::IPointLight::Destroy(m_pLight);
  duckGfx::ISpotLight::Destroy(m_sLight);
  duckGfx::IPointLight::Destroy(m_pLight2);
  duckGfx::ISpotLight::Destroy(m_sLight2);

}