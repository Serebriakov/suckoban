//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"
#include "Material.h"
using namespace Gine;

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

  void SetViewProj(CXMMATRIX M)                      { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                         { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
  void SetWorldView(CXMMATRIX M)                     { WorldView->SetMatrix(reinterpret_cast<const float*>(&M)); }
  void SetWorldViewProj(CXMMATRIX M)                 { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)              { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)             { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
  void SetTexTransform(CXMMATRIX M)                  { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                 { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
  void SetWindowsSize(const XMFLOAT2& v)             { WindowSize->SetRawValue(&v, 0, sizeof(XMFLOAT2)); }
	void SetDirLights(const DirectionalLight* lights)  { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
  void SetDirLight(const DirectionalLight* light)    { DirLight->SetRawValue(light, 0, sizeof(DirectionalLight)); }
  void SetPointLights(const PointLight* lights)      { PointLights->SetRawValue(lights, 0, 50*sizeof(PointLight)); }
  void SetRenderedPointLights(float f)               { RenderedPointLights->SetFloat(f); }
	void SetMaterialColors(const MaterialColors& mat)  { Mat->SetRawValue(&mat, 0, sizeof(MaterialColors)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int n) { BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, n); }
  void SetDiffuseMap(ID3D11ShaderResourceView* tex)  { DiffuseMap->SetResource(tex); }
  void SetBumpMap(ID3D11ShaderResourceView* tex)     { BumpMap->SetResource(tex); }
  void SetSpecularMap(ID3D11ShaderResourceView* tex) { SpecularMap->SetResource(tex); }
  void SetSsaoMap(ID3D11ShaderResourceView* tex)     { SsaoMap->SetResource(tex); }
	
	ID3DX11EffectTechnique* BasicTech;
	ID3DX11EffectTechnique* InstancedTech;
	ID3DX11EffectTechnique* SpriteTech;
	
	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldView;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
  ID3DX11EffectVectorVariable* WindowSize;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* DirLight;
	ID3DX11EffectVariable* PointLights;
	ID3DX11EffectScalarVariable* RenderedPointLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectMatrixVariable* BoneTransforms;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* BumpMap;
	ID3DX11EffectShaderResourceVariable* SpecularMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
};
#pragma endregion

#pragma region SsaoNormalDepthEffect
class SsaoNormalDepthEffect : public Effect
{
public:
	SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoNormalDepthEffect();

	void SetWorldView(CXMMATRIX M)                      { WorldView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTransposeView(CXMMATRIX M)          { WorldInvTransposeView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* NormalDepthTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipTech;

	ID3DX11EffectMatrixVariable* WorldView;
	ID3DX11EffectMatrixVariable* WorldInvTransposeView;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};
#pragma endregion

#pragma region SsaoEffect
class SsaoEffect : public Effect
{
public:
	SsaoEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoEffect();

	void SetViewToTexSpace(CXMMATRIX M)                    { ViewToTexSpace->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetOffsetVectors(const XMFLOAT4 v[14])            { OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14); }
	void SetFrustumCorners(const XMFLOAT4 v[4])            { FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4); }
	void SetOcclusionRadius(float f)                       { OcclusionRadius->SetFloat(f); }
	void SetOcclusionFadeStart(float f)                    { OcclusionFadeStart->SetFloat(f); }
	void SetOcclusionFadeEnd(float f)                      { OcclusionFadeEnd->SetFloat(f); }
	void SetSurfaceEpsilon(float f)                        { SurfaceEpsilon->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetRandomVecMap(ID3D11ShaderResourceView* srv)    { RandomVecMap->SetResource(srv); }

	ID3DX11EffectTechnique* SsaoTech;

	ID3DX11EffectMatrixVariable* ViewToTexSpace;
	ID3DX11EffectVectorVariable* OffsetVectors;
	ID3DX11EffectVectorVariable* FrustumCorners;
	ID3DX11EffectScalarVariable* OcclusionRadius;
	ID3DX11EffectScalarVariable* OcclusionFadeStart;
	ID3DX11EffectScalarVariable* OcclusionFadeEnd;
	ID3DX11EffectScalarVariable* SurfaceEpsilon;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* RandomVecMap;
};
#pragma endregion

#pragma region SsaoBlurEffect
class SsaoBlurEffect : public Effect
{
public:
	SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoBlurEffect();

	void SetTexelWidth(float f)                            { TexelWidth->SetFloat(f); }
	void SetTexelHeight(float f)                           { TexelHeight->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetInputImage(ID3D11ShaderResourceView* srv)      { InputImage->SetResource(srv); }

	ID3DX11EffectTechnique* HorzBlurTech;
	ID3DX11EffectTechnique* VertBlurTech;

	ID3DX11EffectScalarVariable* TexelWidth;
	ID3DX11EffectScalarVariable* TexelHeight;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* InputImage;
};
#pragma endregion

#pragma region DebugTexEffect
class DebugTexEffect : public Effect
{
public:
	DebugTexEffect(ID3D11Device* device, const std::wstring& filename);
	~DebugTexEffect();

	void SetWorldViewProj(CXMMATRIX M)              { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex)  { Texture->SetResource(tex); }

	ID3DX11EffectTechnique* ViewArgbTech;
	ID3DX11EffectTechnique* ViewRedTech;
	ID3DX11EffectTechnique* ViewGreenTech;
	ID3DX11EffectTechnique* ViewBlueTech;
	ID3DX11EffectTechnique* ViewAlphaTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectShaderResourceVariable* Texture;
};
#pragma endregion

#pragma region ParticleEffect
class ParticleEffect : public Effect
{
public:
	ParticleEffect(ID3D11Device* device, const std::wstring& filename);
	~ParticleEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetGameTime(float f)                           { GameTime->SetFloat(f); }
	void SetTimeStep(float f)                           { TimeStep->SetFloat(f); }

	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitPosW(const XMFLOAT3& v)                 { EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitDirW(const XMFLOAT3& v)                 { EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

  void SetColor(const XMFLOAT4& v)                    { Color->SetRawValue(&v, 0, sizeof(XMFLOAT4)); }
	void SetSize(float f)                               { Size->SetFloat(f); }
  void SetTexArray(ID3D11ShaderResourceView* tex)     { TexArray->SetResource(tex); }
	void SetRandomTex(ID3D11ShaderResourceView* tex)    { RandomTex->SetResource(tex); }
	
	ID3DX11EffectTechnique* StreamOutTech;
	ID3DX11EffectTechnique* DrawTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectScalarVariable* GameTime;
	ID3DX11EffectScalarVariable* TimeStep;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* EmitPosW;
	ID3DX11EffectVectorVariable* EmitDirW;
  ID3DX11EffectVectorVariable* Color;
	ID3DX11EffectScalarVariable* Size;
	ID3DX11EffectShaderResourceVariable* TexArray;
	ID3DX11EffectShaderResourceVariable* RandomTex;
};
#pragma endregion

#pragma region SpriteEffect
class SpriteEffect : public Effect
{
public:
	SpriteEffect(ID3D11Device* device, const std::wstring& filename);
	~SpriteEffect();

	void SetSpriteMap(ID3D11ShaderResourceView* tex)   { SpriteMap->SetResource(tex); }

	ID3DX11EffectTechnique* SpriteTech;
	ID3DX11EffectShaderResourceVariable* SpriteMap;
};
#pragma endregion

#pragma region BillboardEffect
class BillboardEffect : public Effect
{
public:
	BillboardEffect(ID3D11Device* device, const std::wstring& filename);
	~BillboardEffect();

	void SetViewProj(CXMMATRIX M)                              { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                         { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	//void SetFogColor(const FXMVECTOR v)                        { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	//void SetFogStart(float f)                                  { FogStart->SetFloat(f); }
	//void SetFogRange(float f)                                  { FogRange->SetFloat(f); }
	//void SetDirLights(const DirectionalLight* lights)          { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	//void SetMaterial(const Material& mat)                      { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetBillboardTextureMapArray(ID3D11ShaderResourceView* tex) { BillboardTextureMapArray->SetResource(tex); }

	ID3DX11EffectTechnique* BillboardTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectVectorVariable* EyePosW;
	//ID3DX11EffectVectorVariable* FogColor;
	//ID3DX11EffectScalarVariable* FogStart;
	//ID3DX11EffectScalarVariable* FogRange;
	//ID3DX11EffectVariable* DirLights;
	//ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* BillboardTextureMapArray;
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect*           BasicFX;
	static SpriteEffect*          SpriteFX;
 // static ParticleEffect*        FireFX;
	static SsaoNormalDepthEffect* SsaoNormalDepthFX;
	static SsaoEffect*            SsaoFX;
	static SsaoBlurEffect*        SsaoBlurFX;
	//static DebugTexEffect*        DebugTexFX;
 // static BillboardEffect*       BillboardFX;
};
#pragma endregion

#endif // EFFECTS_H