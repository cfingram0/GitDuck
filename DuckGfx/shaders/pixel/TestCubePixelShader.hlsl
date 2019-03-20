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

  float4 spotPos; // w is inner angle
  float4 spotDir; // w is outer angle
  float4 spotColor; // w is intensity
}

float4 main(VS_Output input) : SV_TARGET
{
  float gamma = 2.2f;

  float3 n = normalize(input.Normal.xyz);
  float3 toView = normalize(cameraWorldPos.xyz - input.wPosition.xyz);

  float3 totalSpec = float3(0, 0, 0);
  float3 totalDiffuse = float3(0, 0, 0);

  //directional light
  {
    float3 linLightColor = pow(lightColor.xyz, gamma);

    float dirDiffuseDot = dot(lightDir.xyz, n);
    float3 dirDiffuse = dirDiffuseDot > 0 ? dirDiffuseDot : 0;
    totalDiffuse += dirDiffuse * linLightColor.xyz;
    
    float3 dirReflected = reflect(-lightDir.xyz, n);
    float dirSpecDot = dot(toView, dirReflected);
    float dirSpec = dirSpecDot > 0 ? dirSpecDot : 0;
    dirSpec = pow(dirSpec, roughness);
    totalSpec += dirSpec * linLightColor;
  }

  // point light
  {
    float3 linPointColor = pow(pointColor.xyz, gamma);

    float3 toPoint = pointPos.xyz - input.wPosition.xyz;
    float toPointLenSq = dot(toPoint, toPoint);
    toPointLenSq = max(toPointLenSq, 0.0001f);
    toPoint = normalize(toPoint);
    float pIntens = pointColor.w / (toPointLenSq);
    
    float3 pDiffuseDot = dot(toPoint, n);
    float3 pDiffuse = pDiffuseDot > 0 ? pDiffuseDot : 0;
    totalDiffuse += pIntens * pDiffuse * linPointColor;
    
    float3 pReflected = reflect(-toPoint, n);
    float pSpecDot = dot(toView, pReflected);
    float pSpec = pSpecDot > 0 ? pSpecDot : 0;
    pSpec = pow(pSpec, roughness);
    totalSpec += pIntens * pSpec * linPointColor;
  }

  // spot light 
  {
    float3 linSpotColor = pow(spotColor.xyz, gamma);

    float3 toSpot = spotPos.xyz - input.wPosition.xyz;
    float toSpotLenSq = dot(toSpot, toSpot);
    toSpotLenSq = max(toSpotLenSq, 0.0001f);
    toSpot = normalize(toSpot);
    float sIntens = spotColor.w / toSpotLenSq;

    float cosAngle = dot(spotDir.xyz, -toSpot);
    float angleM = 1.0f;
    if (cosAngle > spotPos.w) {
      angleM = 1;
    }
    else if (cosAngle < spotDir.w) {
      angleM = 0;
    }
    else {
      angleM = (cosAngle - spotDir.w) / (spotPos.w - spotDir.w);
    }
    

    float3 sDiffuseDot = dot(toSpot, n);
    float3 sDiffuse = sDiffuseDot > 0 ? sDiffuseDot : 0;
    totalDiffuse += sIntens * angleM * sDiffuse * linSpotColor;

    float3 sReflected = normalize(reflect(-toSpot, n));
    float sSpecDot = dot(toView, sReflected);
    float sSpec = sSpecDot > 0 ? sSpecDot : 0;
    sSpec = pow(sSpec, roughness);
    totalSpec += sIntens * angleM * sSpec * linSpotColor;
  }

  float3 finalColor = totalSpec + (totalDiffuse + ambient.xyz) * color.xyz;
  finalColor = pow(finalColor, 1.0f / gamma);
 
  return float4(finalColor, 1);
}