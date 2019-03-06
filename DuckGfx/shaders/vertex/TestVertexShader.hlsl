cbuffer TRANSFORM_BUFFER : register(b0) {
  matrix mTransform;
};

struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

VS_Output main( float3 pos : POSITION, float3 col : COLOR0 )
{
  VS_Output output;

  output.Position = mul(float4(pos, 1), mTransform);
  output.Color = float4(col, 1);

	return output;
}