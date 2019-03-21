struct VS_Output {
  float4 Position : SV_POSITION;
};

cbuffer MATERIAL_DATA : register(b0) {
  float4 color;
}

float4 main(VS_Output input) : SV_TARGET
{
  return float4(color.xyz, 1);
}