cbuffer TRANSFORM_BUFFER : register(b0) {
  matrix gTransform;
  matrix gNormalTransform;
};

cbuffer PASS_DATA : register(b1) {
  matrix gViewProj;
}

struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Normal : NORMAL;
  float4 wPosition : POSITIONT;
};

VS_Output main(float3 pos : POSITION, float3 n : NORMAL)
{
  VS_Output output;
  matrix transform = mul(gTransform, gViewProj);

  output.Position = mul(float4(pos, 1), transform);

  output.Normal = mul(float4(n, 0), gNormalTransform);
  output.wPosition = mul(float4(pos, 1), gTransform);

  return output;
}