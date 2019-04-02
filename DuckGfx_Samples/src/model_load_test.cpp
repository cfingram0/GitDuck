#include "model_load_test.h"
#include "DuckGfx_utils.h"
#include "input.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

void ModelLoad_Test::Init() {
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


  Assimp::Importer importer;
  const aiScene * scene = importer.ReadFile("uv_sphere.blend", aiProcess_Triangulate);
  for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
    aiMesh * theMesh = scene->mMeshes[i];
    uint32_t numVerts = theMesh->mNumVertices;
  }

}

void ModelLoad_Test::OnStart() {
  m_scene->AddModel(m_model2);
  m_scene->SetMainCamera(m_camera);
}

void ModelLoad_Test::Update(float dt) {
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
    rotAngle += 1 * (3.14) / 180.0f;
  }
  if (input::IsPressed(Key::E)) {
    rotAngle -= 1 * (3.14) / 180.0f;
  }

  Quaternion rotation(rotAngle, Vec3(0, 1, 0));


  m_cameraTrans.translation += offset * 2 * dt;
  m_cameraTrans.rotation = rotation * m_cameraTrans.rotation;
  m_camera->SetTransform(m_cameraTrans);

  m_model2->SetTransform(m_modelTrans2);
  m_time += 2 * dt;
}

void ModelLoad_Test::OnEnd() {
  m_scene->RemoveModel(m_model2);
  m_scene->SetMainCamera(nullptr);
}

void ModelLoad_Test::Shutdown() {
  duckGfx::IModel::Destroy(m_model2);
  m_model2 = nullptr;
  duckGfx::IMaterialInstance::Destroy(m_matInst2);
  m_matInst2 = nullptr;

  duckGfx::IMaterial::Destroy(m_material);
  m_material = nullptr;

  duckGfx::ICamera::Destroy(m_camera);
}