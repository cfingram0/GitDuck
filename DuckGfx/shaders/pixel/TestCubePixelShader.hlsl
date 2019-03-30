struct VS_Output {
  float4 Position : SV_POSITION;
  float4 Normal : NORMAL;
  float4 wPosition : POSITIONT;
};

cbuffer MATERIAL_DATA : register(b0) {
  float4 color;
  float roughness;
  float metalness;
  float2 unused;
}

cbuffer LIGHTING_DATA : register(b1) {
  float4 cameraWorldPos;
  float4 lightDir;
  float4 lightColor;
  float4 ambient;

  uint4  numLights; // x is point lights, y is spot lights
  float4 pointPos[8];
  float4 pointColor[8];

  float4 spotPos[8]; // w is inner angle
  float4 spotDir[8]; // w is outer angle
  float4 spotColor[8]; // w is intensity
}

float3 phongLighting(float3 linLightColor, float3 lightDir, float3 diffColor, float3 normal, float3 toView, float roughness) {
  float normDot = max(0, dot(lightDir.xyz, normal));

  float3 halVec = normalize(lightDir + toView);
  float dirSpecDot = max(0, dot(halVec, normal));
  dirSpecDot = pow(dirSpecDot, roughness);

  return (diffColor + float3(dirSpecDot, dirSpecDot, dirSpecDot)) * linLightColor * normDot;
}

float D(float rSq, float NdotH) {
  float temp = NdotH * NdotH * (rSq * rSq - 1) + 1;
  return (rSq * rSq) / (3.14 * temp * temp);
}

float Gprime(float k, float NdotV) {
  return NdotV / (NdotV * (1 - k) + k);
}

float G(float roughness, float NdotL, float NdotV) {
  float k = ((roughness + 1) * (roughness + 1)) / 8;
  return Gprime(k, NdotL) * Gprime(k, NdotV);
}

float3 F(float3 F0, float VdotH) {
  return F0 + (1 - F0) * pow(2, (-5.55473 * VdotH - 6.98316) * VdotH);
}

float3 brdfLighting(float3 linLightColor, float3 lightDir, float3 diffColor, float3 normal, float3 toView, float roughness, float metalness) {
  
  float normDot = dot(lightDir.xyz, normal);
  if(normDot <= 0)
    return float3(0,0,0);

  float3 halfVec = normalize(lightDir + toView);
  float d = D(roughness * roughness, dot(normal, halfVec));
  float g = G(roughness, dot(normal, lightDir), dot(normal, toView));
  float3 f = F(float3(0.03, 0.03, 0.03), dot(halfVec, toView));
  float3 metalF = F(diffColor, dot(halfVec, toView));

  float specNoF = d * g /(4 * dot(normal, lightDir) * dot(normal, toView));

  float3 dielectricColor = diffColor / 3.14 + specNoF * f;
  float3 metalColor = specNoF * metalF;

  return lerp(dielectricColor, metalColor, metalness) * linLightColor * normDot;
}


float4 main(VS_Output input) : SV_TARGET
{
  const float gamma = 2.2f;

  float3 n = normalize(input.Normal.xyz);
  float3 toView = normalize(cameraWorldPos.xyz - input.wPosition.xyz);

  float3 totalColor = float3(0, 0, 0);
  float3 diffColor = pow(color.xyz, gamma);

  //directional light
  {
    float3 linLightColor = pow(lightColor.xyz, gamma);
    float3 lit = brdfLighting(linLightColor, lightDir.xyz, diffColor, n, toView, roughness, metalness);
    totalColor += lit;
  }

  // point light
  for (uint i = 0; i < numLights.x && i < 8; ++i) {
    float3 linPointColor = pow(pointColor[i].xyz, gamma);
  
    float3 toPoint = pointPos[i].xyz - input.wPosition.xyz;
    float toPointLenSq = dot(toPoint, toPoint);
    toPointLenSq = max(toPointLenSq, 0.0001f);
    toPoint = normalize(toPoint);
    float pIntens = pointColor[i].w / (toPointLenSq + 1);
  
    float3 lit = brdfLighting(linPointColor, toPoint.xyz, diffColor, n, toView, roughness, metalness);
    totalColor += lit * pIntens;
  }
  
  //// spot light 
  for (uint j = 0; j < numLights.y && j < 8; ++j) {
    float3 linSpotColor = pow(spotColor[j].xyz, gamma);
  
    // calculate intensity
    float3 toSpot = spotPos[j].xyz - input.wPosition.xyz;
    float toSpotLenSq = dot(toSpot, toSpot);
    toSpotLenSq = max(toSpotLenSq, 0.0001f);
    toSpot = normalize(toSpot);
    float sIntens = spotColor[j].w / (toSpotLenSq + 1);
  
    //calculate spotlight falloff
    float cosAngle = dot(spotDir[j].xyz, -toSpot);
    float angleM = 1.0f;
    if (cosAngle > spotPos[j].w) {
      angleM = 1;
    }
    else if (cosAngle < spotDir[j].w) {
      angleM = 0;
    }
    else {
      angleM = (cosAngle - spotDir[j].w) / (spotPos[j].w - spotDir[j].w);
    }
    angleM = max(angleM, 0);
    
  
    float3 lit = brdfLighting(linSpotColor, toSpot.xyz, diffColor, n, toView, roughness, metalness);
    totalColor += lit * sIntens * angleM;
  }
  
  float3 finalColor = totalColor + ambient.xyz * diffColor;
  return float4(finalColor, 1);
}