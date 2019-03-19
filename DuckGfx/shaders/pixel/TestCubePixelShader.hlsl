struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Normal : NORMAL;
  float4 wPosition : POSITIONT;
};

cbuffer MATERIAL_DATA : register(b0) {
  float4 color;
  float roughness;
  float3 unused;
}

cbuffer LIGHTING_DATA : register(b1) {
  float4 cameraWorldPos;
  float4 lightDir;
  float4 lightColor;
  float4 ambient;

  float4 pointPos;
  float4 pointColor;
}

float4 main(VS_Output input) : SV_TARGET
{
  float3 n = normalize(input.Normal.xyz);
  float3 toView = normalize(cameraWorldPos.xyz - input.wPosition.xyz);

  float3 totalSpec = float3(0, 0, 0);
  float3 totalDiffuse = float3(0, 0, 0);

  //directional light
  float dirDiffuseDot = dot(lightDir.xyz, n);
  float3 dirDiffuse = dirDiffuseDot > 0 ? dirDiffuseDot : 0;
  totalDiffuse += dirDiffuse * lightColor.xyz;
  
  float3 dirReflected = reflect(-lightDir, n);
  float dirSpecDot = dot(toView, dirReflected);
  float dirSpec = dirSpecDot > 0 ? dirSpecDot : 0;
  dirSpec = pow(dirSpec, roughness);

  totalSpec += dirSpec * lightColor.xyz;


  // point light
  float3 toPoint = pointPos.xyz - input.wPosition.xyz;
  float toPointLenSq = dot(toPoint, toPoint);
  toPointLenSq = max(toPointLenSq, 0.0001f);
  toPoint = normalize(toPoint);
  float pIntens = pointColor.w / (toPointLenSq);

  float3 pDiffuseDot = dot(toPoint, n);
  float3 pDiffuse = pDiffuseDot > 0 ? pDiffuseDot : 0;
  totalDiffuse += pIntens * pDiffuse * pointColor;
  
  float3 pReflected = reflect(-toPoint, n);
  float pSpecDot = dot(toView, pReflected);
  float pSpec = pSpecDot > 0 ? pSpecDot : 0;
  pSpec = pow(pSpec, roughness);

  totalSpec += pIntens * pSpec * pointColor;
 
  return float4((totalSpec + totalDiffuse + ambient.xyz) * color.xyz, 1);
}