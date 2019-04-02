#pragma once
#include "DuckGfx.h"

namespace duckGfx {

  IModel * GenerateDebugTriangleModel();
  IMaterial * GenerateDebugMaterial();

  IModel * GenerateDebugCubeModel();
  IMaterial * GenerateDebugCubeMaterial();

  IMaterial * GenerateDebugDrawMaterial();

  IModel * CreateMesh(VertexFormat fmt, void * vertBuffer, uint64_t numVerts, uint32_t * idxBuffer, uint32_t numIdx);
}