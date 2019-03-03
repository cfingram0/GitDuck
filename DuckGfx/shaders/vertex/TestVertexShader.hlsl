struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

VS_Output main( float3 pos : POSITION, float3 col : COLOR0 )
{
  VS_Output output;

  output.Position = float4(pos, 1);
  output.Color = float4(col, 1);

	return output;
}