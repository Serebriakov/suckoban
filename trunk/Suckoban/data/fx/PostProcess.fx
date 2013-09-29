//------------------------------------------------------------------------------
// CONSTANT BUFFERS
//------------------------------------------------------------------------------

cbuffer cbConst
{
  const float gBlurRadius = 20;
  //float blurWeights[9] = {0.05f, 0.09f, 0.12f, 0.15f, 0.16f, 0.15f, 0.12f, 0.09f, 0.05f};
  //float blurWeights[9] = {0.111f, 0.111f, 0.111f, 0.111f, 0.111f, 0.111f, 0.111f, 0.111f, 0.111f};
  const float blurWeights[41] = { 
      0.00562570633038035, 0.006836982710076046, 0.008226383671597696, 0.009799648425192407, 0.011557637918462808, 
      0.013495367927063531, 0.015601179339011327, 0.01785612363276015, 0.02023363887223212, 0.022699581996180204, 
      0.02521266657379665, 0.027725332059217345, 0.03018504219275014, 0.032535978573349215, 0.03472106315172031, 
      0.03668421340450519, 0.03837270923994921, 0.039739533968928785, 0.04074554506093683, 0.04136133511624546, 
      0.04156865967128831, 0.04136133511624546, 0.04074554506093683, 0.039739533968928785, 0.03837270923994921, 
      0.03668421340450519, 0.03472106315172031, 0.032535978573349215, 0.03018504219275014, 0.027725332059217345, 
      0.02521266657379665, 0.022699581996180204, 0.02023363887223212, 0.01785612363276015, 0.015601179339011327, 
      0.013495367927063531, 0.011557637918462808, 0.009799648425192407, 0.008226383671597696, 0.006836982710076046, 
      0.00562570633038035
   };
}

cbuffer cbPerFrame
{
	float2 gWindowSize;
  float  gTransition;
};

//------------------------------------------------------------------------------
// TEXTURES
//------------------------------------------------------------------------------

Texture2D Screenshot;

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

BlendState BSAdd
{
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = SRC_ALPHA;
    DestBlendAlpha = DEST_ALPHA;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

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
	return Screenshot.Sample(samAnisotropic, pin.Tex);
}

float4 GreyscalePS(VertexOut pin) : SV_Target
{
	float4 sampl = Screenshot.Sample(samAnisotropic, pin.Tex);

  float avg = (sampl.r + sampl.g + sampl.b) / 3.0f;
  float4 effect = float4(avg, avg, avg, 1.0f);

  return sampl * (1.0f - gTransition) + effect * gTransition;
}

float4 BlurHPS(VertexOut pin) : SV_Target
{
	float4 sampl = Screenshot.Sample(samAnisotropic, pin.Tex);

  float size = 1.0f / gWindowSize.x;
  float4 effect = 0.0f;
  for(int i=0; i<gBlurRadius * 2 + 1; i++) {
    effect += Screenshot.Sample(samAnisotropic, float2(pin.Tex.x + (i-gBlurRadius) * size, pin.Tex.y)) * blurWeights[i];
  }

  return sampl * (1.0f - gTransition) + effect * gTransition;
}

float4 BlurVPS(VertexOut pin) : SV_Target
{
	float4 sampl = Screenshot.Sample(samAnisotropic, pin.Tex);

  float size = 1.0f / gWindowSize.y;
  float4 effect = 0.0f;
  for(int i=0; i<gBlurRadius * 2 + 1; i++) {
    effect += Screenshot.Sample(samAnisotropic, float2(pin.Tex.x, pin.Tex.y + (i-gBlurRadius) * size)) * blurWeights[i];
  }
  
  return sampl * (1.0f - gTransition) + effect * gTransition;
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

technique11 GreyscaleTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, GreyscalePS()));
  }
}

technique11 BlurHTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, BlurHPS()));
  }
}

technique11 BlurVTech
{
  pass P0
  {
    SetVertexShader   (CompileShader(vs_4_0, DefaultVS()));
    SetGeometryShader (NULL);
    SetPixelShader    (CompileShader(ps_4_0, BlurVPS()));
  }
}