//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BasicTech     = mFX->GetTechniqueByName("BasicTech");
	InstancedTech = mFX->GetTechniqueByName("InstancedTech");
	SpriteTech    = mFX->GetTechniqueByName("SpriteTech");
	AplaTech      = mFX->GetTechniqueByName("AplaTech");

  ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldView         = mFX->GetVariableByName("gWorldView")->AsMatrix();
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	WindowSize        = mFX->GetVariableByName("gWindowSize")->AsVector();
	DirLights         = mFX->GetVariableByName("gDirLights");
	DirLight          = mFX->GetVariableByName("gDirLight");
	PointLights       = mFX->GetVariableByName("gPointLights");
  RenderedPointLights=mFX->GetVariableByName("gRenderedPointLights")->AsScalar();
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	BumpMap           = mFX->GetVariableByName("gBumpMap")->AsShaderResource();
	SpecularMap       = mFX->GetVariableByName("gSpecularMap")->AsShaderResource();
  SsaoMap           = mFX->GetVariableByName("gSsaoMap")->AsShaderResource();
  BoneTransforms    = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region SsaoNormalDepthEffect
SsaoNormalDepthEffect::SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	NormalDepthTech          = mFX->GetTechniqueByName("NormalDepth");
	NormalDepthAlphaClipTech = mFX->GetTechniqueByName("NormalDepthAlphaClip");

	WorldView             = mFX->GetVariableByName("gWorldView")->AsMatrix();
	WorldInvTransposeView = mFX->GetVariableByName("gWorldInvTransposeView")->AsMatrix();
	WorldViewProj         = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform          = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap            = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

SsaoNormalDepthEffect::~SsaoNormalDepthEffect()
{
}
#pragma endregion

#pragma region SsaoEffect
SsaoEffect::SsaoEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SsaoTech           = mFX->GetTechniqueByName("Ssao");

	ViewToTexSpace     = mFX->GetVariableByName("gViewToTexSpace")->AsMatrix();
	OffsetVectors      = mFX->GetVariableByName("gOffsetVectors")->AsVector();
	FrustumCorners     = mFX->GetVariableByName("gFrustumCorners")->AsVector();
	OcclusionRadius    = mFX->GetVariableByName("gOcclusionRadius")->AsScalar();
	OcclusionFadeStart = mFX->GetVariableByName("gOcclusionFadeStart")->AsScalar();
	OcclusionFadeEnd   = mFX->GetVariableByName("gOcclusionFadeEnd")->AsScalar();
	SurfaceEpsilon     = mFX->GetVariableByName("gSurfaceEpsilon")->AsScalar();

	NormalDepthMap     = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	RandomVecMap       = mFX->GetVariableByName("gRandomVecMap")->AsShaderResource();
}

SsaoEffect::~SsaoEffect()
{
}
#pragma endregion

#pragma region SsaoBlurEffect
SsaoBlurEffect::SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	HorzBlurTech    = mFX->GetTechniqueByName("HorzBlur");
	VertBlurTech    = mFX->GetTechniqueByName("VertBlur");

	TexelWidth      = mFX->GetVariableByName("gTexelWidth")->AsScalar();
	TexelHeight     = mFX->GetVariableByName("gTexelHeight")->AsScalar();

	NormalDepthMap  = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	InputImage      = mFX->GetVariableByName("gInputImage")->AsShaderResource();
}

SsaoBlurEffect::~SsaoBlurEffect()
{
}
#pragma endregion

#pragma region DebugTexEffect
DebugTexEffect::DebugTexEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	ViewArgbTech  = mFX->GetTechniqueByName("ViewArgbTech");
	ViewRedTech   = mFX->GetTechniqueByName("ViewRedTech");
	ViewGreenTech = mFX->GetTechniqueByName("ViewGreenTech");
	ViewBlueTech  = mFX->GetTechniqueByName("ViewBlueTech");
	ViewAlphaTech = mFX->GetTechniqueByName("ViewAlphaTech");

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	Texture       = mFX->GetVariableByName("gTexture")->AsShaderResource();
}

DebugTexEffect::~DebugTexEffect()
{
}
#pragma endregion

#pragma region ParticleEffect
ParticleEffect::ParticleEffect(ID3D11Device* device, 
                               const std::wstring& filename)
                              : Effect(device, filename)
{
	StreamOutTech    = mFX->GetTechniqueByName("StreamOutTech");
	DrawTech         = mFX->GetTechniqueByName("DrawTech");

	ViewProj    = mFX->GetVariableByName("gViewProj")->AsMatrix();
	GameTime    = mFX->GetVariableByName("gGameTime")->AsScalar();
	TimeStep    = mFX->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW     = mFX->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW    = mFX->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW    = mFX->GetVariableByName("gEmitDirW")->AsVector();
  Color       = mFX->GetVariableByName("gColor")->AsVector();
  Size        = mFX->GetVariableByName("gSize")->AsScalar();
	TexArray    = mFX->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex   = mFX->GetVariableByName("gRandomTex")->AsShaderResource();
}

ParticleEffect::~ParticleEffect()
{
}
#pragma endregion

#pragma region SpriteEffect
SpriteEffect::SpriteEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SpriteTech = mFX->GetTechniqueByName("SpriteTech");
	SpriteMap  = mFX->GetVariableByName("gSpriteTex")->AsShaderResource();
}

SpriteEffect::~SpriteEffect()
{
}
#pragma endregion

#pragma region PostProcessEffect
PostProcessEffect::PostProcessEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
  DefaultTech   = mFX->GetTechniqueByName("DefaultTech");
  GreyscaleTech = mFX->GetTechniqueByName("GreyscaleTech");
  BlurHTech     = mFX->GetTechniqueByName("BlurHTech");
  BlurVTech     = mFX->GetTechniqueByName("BlurVTech");

	Screenshot    = mFX->GetVariableByName("Screenshot")->AsShaderResource();
  Transition    = mFX->GetVariableByName("gTransition")->AsScalar();
	WindowSize    = mFX->GetVariableByName("gWindowSize")->AsVector();
}
#pragma endregion

#pragma region BillboardEffect
BillboardEffect::BillboardEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BillboardTech       = mFX->GetTechniqueByName("BillboardTech");

	ViewProj            = mFX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW             = mFX->GetVariableByName("gEyePosW")->AsVector();
	//FogColor            = mFX->GetVariableByName("gFogColor")->AsVector();
	//FogStart            = mFX->GetVariableByName("gFogStart")->AsScalar();
	//FogRange            = mFX->GetVariableByName("gFogRange")->AsScalar();
	//DirLights           = mFX->GetVariableByName("gDirLights");
	//Mat                 = mFX->GetVariableByName("gMaterial");
	BillboardTextureMapArray = mFX->GetVariableByName("gBillboardMapArray")->AsShaderResource();
}

BillboardEffect::~BillboardEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect*    Effects::BasicFX = 0;
SpriteEffect*   Effects::SpriteFX = 0;
PostProcessEffect* Effects::PostProcessFX = 0;
//ParticleEffect* Effects::FireFX = 0;
SsaoNormalDepthEffect* Effects::SsaoNormalDepthFX = 0;
SsaoEffect*            Effects::SsaoFX            = 0;
SsaoBlurEffect*        Effects::SsaoBlurFX        = 0;
//DebugTexEffect*        Effects::DebugTexFX        = 0;
//BillboardEffect*        Effects::BillboardFX        = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX              = new BasicEffect   (device, L"data/fx/Basic.fxo");
	SpriteFX             = new SpriteEffect  (device, L"data/fx/Sprite.fxo"); 
  PostProcessFX = new PostProcessEffect(device, L"data/fx/PostProcess.fxo");
	//FireFX               = new ParticleEffect(device, L"FX/Fire.fxo"); 

  SsaoNormalDepthFX = new SsaoNormalDepthEffect(device, L"data/fx/SsaoNormalDepth.fxo");
	SsaoFX            = new SsaoEffect(device, L"data/fx/Ssao.fxo");
	SsaoBlurFX        = new SsaoBlurEffect(device, L"data/fx/SsaoBlur.fxo");
 // DebugTexFX        = new DebugTexEffect(device, L"FX/DebugTexture.fxo");

 // BillboardFX      = new BillboardEffect(device, L"FX/Billboard.fxo");

}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(SpriteFX);
  SafeDelete(PostProcessFX);
	//SafeDelete(FireFX);

  SafeDelete(SsaoNormalDepthFX);
	SafeDelete(SsaoFX);
	SafeDelete(SsaoBlurFX);
	//SafeDelete(DebugTexFX);

 // SafeDelete(BillboardFX);
}
#pragma endregion