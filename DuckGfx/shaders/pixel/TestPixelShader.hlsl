struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Color : COLOR;
};

float4 main(VS_Output input) : SV_TARGET
{
	return input.Color;
}