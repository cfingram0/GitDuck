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
}

float4 main(VS_Output input) : SV_TARGET
{

  float3 n = normalize(input.Normal.xyz);
  float3 lightDir = normalize(float3(1, 1, 1));
  float diffuseDot = dot(lightDir, n);
  float3 diffuse = diffuseDot > 0 ? diffuseDot : 0;
  float3 ambient = float3(0.2, 0.2, 0.2);

  float3 reflected = normalize(2 * n - lightDir);
  float3 toView = normalize(cameraWorldPos.xyz - input.wPosition.xyz);
  float specDot = dot(toView, reflected);
  float spec = specDot > 0 ? specDot : 0;
  spec = pow(spec, roughness);

  return float4(float3(spec, spec, spec) + (diffuse + ambient) * color.xyz, 1);
}