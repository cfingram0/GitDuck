cbuffer TRANSFORM_BUFFER : register(b0) {
  matrix gTransform;
};

struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

VS_Output main( float3 pos : POSITION, float4 col : COLOR0 )
{
  VS_Output output;

  output.Position = mul(float4(pos, 1), gTransform);
  output.Color = col;

	return output;
}