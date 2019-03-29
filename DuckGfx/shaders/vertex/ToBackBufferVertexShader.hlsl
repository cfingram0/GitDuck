struct VS_Output {
  float4 Position : SV_POSITION;
  float2 TexCoord : TEXCOORD0;
};

VS_Output main( float3 pos : POSITION, float2 tex : TEXCOORD0 )
{
  VS_Output output;
  output.Position = float4(pos, 1);
  output.TexCoord = tex;

	return output;
}