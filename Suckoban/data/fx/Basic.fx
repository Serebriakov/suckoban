#include "LightHelper.fx"

//------------------------------------------------------------------------------
// BUFFERS
//------------------------------------------------------------------------------

cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	float3 gEyePosW;
	float2 gWindowSize;
};

cbuffer cbPerObject
{
	float4x4 gViewProj;
	float4x4 gWorld;
  float4x4 gWorldView;
	float4x4 gWorldViewProj;
	float4x4 gWorldViewProjTex;
	float4x4 gWorldInvTranspose;

	float4x4 gTexTransform;
	MaterialColors gMaterial;

  float4x4 gBoneTransforms[96];
}; 

cbuffer cbConstant
{
  float4 gZeroF4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
  float4 gOneF4 = float4(1.0f, 1.0f, 1.0f, 1.0f);

  float  gFogStart = 0.0f;
  float  gFogRange = 10.0f;
  float4 gFogColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
};

//------------------------------------------------------------------------------
// TEXTURES
//------------------------------------------------------------------------------

Texture2D gDiffuseMap;
Texture2D gBumpMap;
Texture2D gSpecularMap;
Texture2D gSsaoMap;
SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

//------------------------------------------------------------------------------
// STATES
//------------------------------------------------------------------------------

DepthStencilState DSSEquals
{
  DepthEnable = true;
  DepthWriteMask = ZERO;
	DepthFunc = EQUAL;
};

//------------------------------------------------------------------------------
// DATA STRUCTURES
//------------------------------------------------------------------------------

struct VertexIn
{
	float3 PosL        : POSITION;
	float3 NormalL     : NORMAL;
	float2 Tex         : TEXCOORD;
	float3 TangentL    : TANGENT;
	float3 Weights     : WEIGHTS;
	uint4  BoneIndices : BONEINDICES;
};

struct VertexInstancedIn
{
	float3 PosL        : POSITION;
	float3 NormalL     : NORMAL;
	float2 Tex         : TEXCOORD;
	float3 TangentL    : TANGENT;
	float3 Weights     : WEIGHTS;
	uint4  BoneIndices : BONEINDICES;
	row_major float4x4 World  : WORLD;
	uint   InstanceId  : SV_InstanceID;
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
  float3 PosW     : POSITION;
  float3 NormalW  : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex      : TEXCOORD0;
	float4 SsaoPosH : TEXCOORD1;
};

//------------------------------------------------------------------------------
// VERTEX SHADERS
//------------------------------------------------------------------------------

VertexOut DefaultVS(VertexIn vin)
{
	VertexOut vout;

  // Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
  float3 norL = float3(0.0f, 0.0f, 0.0f);
  float3 tanL = float3(0.0f, 0.0f, 0.0f);
	for(int i=0; i<4; ++i)
	{
	  // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.

	  posL += weights[i] * mul(float4(vin.PosL, 1.0f),
                             gBoneTransforms[vin.BoneIndices[i]]).xyz;
    norL += weights[i] * mul(vin.NormalL,
                            (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
    tanL += weights[i] * mul(vin.TangentL,
                            (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
	}

	// Transform to world space space.
	vout.PosW     = mul(float4(posL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(norL, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(tanL, (float3x3)gWorld);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

  // Generate projective tex-coords to project SSAO map onto scene.
	vout.SsaoPosH = mul(float4(posL, 1.0f), gWorldViewProjTex);

	return vout;
}

VertexOut InstancedVS(VertexInstancedIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW     = mul(float4(vin.PosL, 1.0f), vin.World).xyz;
	vout.NormalW  = mul(vin.NormalL, (float3x3)vin.World);
	vout.TangentW = mul(vin.TangentL, (float3x3)vin.World);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

  // Generate projective tex-coords to project SSAO map onto scene.
	vout.SsaoPosH = mul(vout.PosH, gWorldViewProjTex);

	return vout;
}

VertexOut SpriteVS(VertexIn vin)
{
  VertexOut vout;
	
	vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(vin.NormalL,  (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);

	vout.PosH = float4(vout.PosW, 1.0f);
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

//------------------------------------------------------------------------------
// PIXEL SHADERS
//------------------------------------------------------------------------------
 
float4 DefaultPS(VertexOut pin,
                 uniform bool uDiffuseMap,
                 uniform bool uNormalMap,
                 uniform bool uSpecularMap,
                 uniform bool uLights,
                 uniform bool uFog,
                 uniform bool uAlphaClip,
                 uniform bool uSSAO
                 ) : SV_Target
{
  // Interpolating normal can unnormalize it, so normalize it.
  pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
  // Default to multiplicative identity.
  float4 texColor = gOneF4;
  if(uDiffuseMap)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );

		if(uAlphaClip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texColor.a - 0.1f);
		}
	}

	// Normal mapping
	float3 normalMapSample;
  if(uNormalMap)
  {
    normalMapSample = gBumpMap.Sample(samLinear, pin.Tex).rgb;
	  pin.NormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW,
                                           pin.TangentW);
  }

	// Lighting
	float4 litColor = texColor;
	if(uLights)
	{
		// Start with a sum of zero.
		float4 ambient = gZeroF4;
		float4 diffuse = gZeroF4;
		float4 spec    = gZeroF4;

    // Finish texture projection and sample SSAO map.
    float ambientAccess = 1.0f;
    if(uSSAO)
    {
		  pin.SsaoPosH /= pin.SsaoPosH.w;
		  ambientAccess = gSsaoMap.SampleLevel(samLinear, pin.SsaoPosH.xy, 0.0f).r;
    }

		// Sum the light contribution from each light source.  
		float4 A, D, S;
		ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEye, A, D, S);

		ambient += A;
		diffuse += D;
		spec    += S;

    // Specular map
    if(uSpecularMap)
    {
	    spec *= gSpecularMap.Sample(samLinear, pin.Tex);
    }

		// Modulate with late add.
		litColor = texColor * (ambient + diffuse) * ambientAccess + spec;
	}

  // Fogging
  if(uFog)
  {
    float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
    litColor = lerp(litColor, gFogColor, fogLerp);
  }

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

  return litColor;
}

float4 SpritePS(VertexOut pin) : SV_Target
{
	return gDiffuseMap.Sample(samAnisotropic, pin.Tex);
}

//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique11 BasicTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, DefaultPS(
      true, false, false, true, false, false, false)));
  }
}

technique11 InstancedTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, InstancedVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, DefaultPS(
      true, false, false, true, false, false, false)));
  }
}

technique11 SpriteTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, SpriteVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, SpritePS()));
  }
}