//------------------------------------------------------------------------------
// CONSTANT BUFFERS
//------------------------------------------------------------------------------

cbuffer cbConst
{
}

cbuffer cbPerFrame
{
	float2 gWindowSize;
  float  gTransition;
};

//------------------------------------------------------------------------------
// TEXTURES
//------------------------------------------------------------------------------

Texture2D From;
Texture2D To;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = CLAMP;
	AddressV = CLAMP;
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

//------------------------------------------------------------------------------
// DATA STRUCTURES
//------------------------------------------------------------------------------

struct VertexIn
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//------------------------------------------------------------------------------
// VERTEX SHADERS
//------------------------------------------------------------------------------

VertexOut DefaultVS(VertexIn vin)
{
  VertexOut vout;
  vout.Pos = float4(vin.Pos, 1.0f);
  vout.Tex = vin.Tex;
  return vout;
}

//------------------------------------------------------------------------------
// PIXEL SHADERS
//------------------------------------------------------------------------------

float4 DefaultPS(VertexOut pin) : SV_Target
{
	return To.Sample(samAnisotropic, pin.Tex);
}

float4 FadeInPS(VertexOut pin) : SV_Target
{
  pin.Tex.x -= (1.0f - gTransition);
  float4 smpl = To.Sample(samAnisotropic, pin.Tex);
  return smpl;
  //return float4(smpl.rgb, smpl.a * gTransition);
}

float4 FadeOutPS(VertexOut pin) : SV_Target
{
  pin.Tex.x -= gTransition;
  float4 smpl = To.Sample(samAnisotropic, pin.Tex);
  return smpl;
  //return float4(smpl.rgb, smpl.a * gTransition);
}

//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique11 DefaultTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, DefaultPS()));
  }
}

technique11 FadeInTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, FadeInPS()));
  }
}

technique11 FadeOutTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, FadeOutPS()));
  }
}