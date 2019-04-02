#include <cassert>

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
  m_matInst2->SetParameter("roughness", 0.9f);

  m_modelTrans2 = TransformSRT(Vec3(10, 1, 10),
                               Quaternion(tag::Identity{}),
                               Vec3(0, -2, -5));

  m_scene->SetAmbientColor(Vec3(0.05f, 0.05f, 0.05f));
  m_scene->SetLightDir(Vec3(1, 1, 0));
  m_scene->SetlightColor(Vec3(0.9f, 0.9f, 0.9f));


  Assimp::Importer importer;
  const aiScene * scene = importer.ReadFile("uv_sphere.blend", 0);
  aiMesh * theMesh = scene->mMeshes[0];
  uint32_t numVerts = theMesh->mNumVertices;
  
  struct vert {
    Vec3 position{ tag::NoInit{} };
    Vec3 normal{ tag::NoInit{} };
  };
  duckGfx::VertexFormat fmt;
  fmt.hasPosition = 1;
  fmt.hasNormals = 1;

  vert * vertBuffer = new vert[numVerts];
  for (uint32_t i = 0; i < numVerts; ++i) {
    vertBuffer[i].position = Vec3(theMesh->mVertices[i].x, theMesh->mVertices[i].y, theMesh->mVertices[i].z);
    vertBuffer[i].normal = Vec3(theMesh->mNormals[i].x, theMesh->mNormals[i].y, theMesh->mNormals[i].z);
  }

  uint32_t idxCount = theMesh->mNumFaces * 3;
  uint32_t * idxBuffer = new uint32_t[idxCount];
  uint32_t idxCounter = 0;
  for (uint32_t i = 0; i < theMesh->mNumFaces; ++i) {
    aiFace & face = theMesh->mFaces[i];
    assert(face.mNumIndices == 3);
    for (uint32_t j = 0; j < 3; ++j) {
      idxBuffer[idxCounter] = face.mIndices[j];
      idxCounter++;
    }
  }

  m_model3 = duckGfx::CreateMesh(fmt, vertBuffer, numVerts, idxBuffer, idxCount);
  delete[] vertBuffer;
  delete[] idxBuffer;


  m_matInst3 = duckGfx::IMaterialInstance::Create(m_material);
  m_model3->SetMaterialInstance(m_matInst3);

  m_matInst3->SetParameter("color", Vec4(1, .7, .7, 1));
  m_matInst3->SetParameter("roughness", 0.2f);

  m_modelTrans3 = TransformSRT(Vec3(1, 1, 1),
                               Quaternion(tag::Identity{}),
                               Vec3(0, 0, -5));

  m_model3->SetTransform(m_modelTrans3);

}

void ModelLoad_Test::OnStart() {
  m_scene->AddModel(m_model2);
  m_scene->SetMainCamera(m_camera);
  m_scene->AddModel(m_model3);
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

  duckGfx::IModel::Destroy(m_model3);
  m_model3 = nullptr;
  duckGfx::IMaterialInstance::Destroy(m_matInst3);
  m_matInst3 = nullptr;

  duckGfx::IMaterial::Destroy(m_material);
  m_material = nullptr;

  duckGfx::ICamera::Destroy(m_camera);
}