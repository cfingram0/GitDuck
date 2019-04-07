struct VS_Output {
  float4 Position : SV_POSITION;
  float2 TexCoord : TEXCOORD0;
};

cbuffer PASS_DATA : register(b0) {
  float4 AvgColor;
}

Texture2D shaderTexture;
SamplerState shaderSampler;

float4 main(VS_Output input) : SV_TARGET
{
  //const float gamma = 2.2f;
  //finalColor = pow(finalColor, 1.0f / gamma);
  //return float4(finalColor, 1);
  //float4 color = float4(0,0,0,1);
  //if (input.TexCoord.x > 0.9)
  //  color.x = 1;
  //if (input.TexCoord.y > 0.9)
  //  color.y = 1;

  float4 finalColor = shaderTexture.Sample(shaderSampler, input.TexCoord);
  finalColor = pow(finalColor, 1.0f / 2.2f);

  return finalColor;
}