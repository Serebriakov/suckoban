#include "PostProcess.h"
#include "GeometryGenerator.h"
#include "Effects.h"
using namespace Gine;

map<PostProcessFilter, vector<ID3DX11EffectTechnique*>> PostProcess::mAllFilters;
map<Transition, vector<ID3DX11EffectTechnique*>> PostProcess::mAllTransitions;
ID3D11RenderTargetView*   PostProcess::mBufferedRenderTargetView[2];
ID3D11ShaderResourceView* PostProcess::mBufferedShaderResourceView[2];
ID3D11Buffer*             PostProcess::mScreenQuadVB = 0;
ID3D11Buffer*             PostProcess::mScreenQuadIB = 0;

bool PostProcess::Init()
{
  CreateBackBufferViews();
  CreateBackBufferBuffers();

  vector<ID3DX11EffectTechnique*> effectTech;

  // Post-processing techniques
  effectTech.push_back(Effects::PostProcessFX->DefaultTech);
  mAllFilters[POST_PROCESS_FILTER_NONE] = effectTech;

  effectTech.clear();
  effectTech.push_back(Effects::PostProcessFX->GreyscaleTech);
  mAllFilters[POST_PROCESS_FILTER_GREYSCALE] = effectTech;

  effectTech.clear();
  effectTech.push_back(Effects::PostProcessFX->BlurHTech);
  effectTech.push_back(Effects::PostProcessFX->BlurVTech);
  mAllFilters[POST_PROCESS_FILTER_BLUR] = effectTech;

  // Transition techniques
  effectTech.clear();
  effectTech.push_back(Effects::TransitionFX->DefaultTech);
  mAllTransitions[TRANSITION_NONE] = effectTech;

  effectTech.clear();
  effectTech.push_back(Effects::TransitionFX->FadeInTech);
  mAllTransitions[TRANSITION_FADEIN] = effectTech;

  effectTech.clear();
  effectTech.push_back(Effects::TransitionFX->FadeOutTech);
  mAllTransitions[TRANSITION_FADEOUT] = effectTech;

  return true;
}

void PostProcess::Destroy()
{
  ReleaseCOM(mScreenQuadVB);
  ReleaseCOM(mScreenQuadIB);
  ReleaseCOM(mBufferedRenderTargetView[0]);
  ReleaseCOM(mBufferedShaderResourceView[0]);
  ReleaseCOM(mBufferedRenderTargetView[1]);
  ReleaseCOM(mBufferedShaderResourceView[1]);
}

void PostProcess::Tick(float dt)
{
  if(mPostProcessFilter != POST_PROCESS_FILTER_NONE) {
    mPostProcessTween.Tick(dt);

    // If post-process transition is finished and effects cleared - set technique to default
    // GetValue have to return 0 right now, but we are not comparing floats of course
    if(!mPostProcessTween.IsOn() && mPostProcessTween.GetValue() < 0.1f)
      mPostProcessFilter = POST_PROCESS_FILTER_NONE;
  }

  if(mTransition != TRANSITION_NONE) {
    mTransitionTween.Tick(dt);

    // If transition is finished and effects cleared - set technique to default
    // GetValue have to return 1 right now, but we are not comparing floats of course
    if(!mTransitionTween.IsOn() && mTransitionTween.GetValue() > 0.9f)
      mTransition = TRANSITION_NONE;
  }
}

void PostProcess::CreateBackBufferViews()
{
  ID3D11Texture2D* texture0 = Gine::Utils::CreateDefaultTexture2D();
  HR(gDevice->CreateShaderResourceView(texture0,  0, &mBufferedShaderResourceView[0]));
	HR(gDevice->CreateRenderTargetView  (texture0,  0, &mBufferedRenderTargetView[0]));

  ID3D11Texture2D* texture1 = Gine::Utils::CreateDefaultTexture2D();
  HR(gDevice->CreateShaderResourceView(texture1,  0, &mBufferedShaderResourceView[1]));
	HR(gDevice->CreateRenderTargetView  (texture1,  0, &mBufferedRenderTargetView[1]));

	ReleaseCOM(texture0);
	ReleaseCOM(texture1);
}

void PostProcess::CreateBackBufferBuffers()
{
  // Geometry generator for fullscreen quad
  GeometryGenerator::MeshData quad;
  GeometryGenerator geoGen;
  geoGen.CreateFullscreenQuad(quad);

  // Copy vertices from generator to Apla struct
  vector<Vertex::Apla> vertices(quad.Vertices.size());
  for(UINT i=0; i < quad.Vertices.size(); ++i) {
	  vertices[i].Pos = quad.Vertices[i].Position;
	  vertices[i].Tex = quad.Vertices[i].TexC;
  }

  // Vertex buffer
  D3D11_BUFFER_DESC vbd;
  vbd.Usage = D3D11_USAGE_IMMUTABLE;
  vbd.ByteWidth = sizeof(Vertex::Apla) * vertices.size();
  vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbd.CPUAccessFlags = 0;
  vbd.MiscFlags = 0;
  D3D11_SUBRESOURCE_DATA vinitData;
  vinitData.pSysMem = &vertices[0];
  HR(gDevice->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB));

  // Index buffer
  D3D11_BUFFER_DESC ibd;
  ibd.Usage = D3D11_USAGE_IMMUTABLE;
  ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.CPUAccessFlags = 0;
  ibd.MiscFlags = 0;
  D3D11_SUBRESOURCE_DATA iinitData;
  iinitData.pSysMem = &quad.Indices[0];
  HR(gDevice->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB));
}

void PostProcess::ClearAndRenderTo(ID3D11RenderTargetView* RTV)
{
  gContext->OMSetRenderTargets(1, &RTV, gDepthStencilView);
	gContext->ClearRenderTargetView(RTV, reinterpret_cast<const float*>(&Colors::MPink));
  gContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void PostProcess::SetPostProcessFilter(PostProcessFilter postProcessFilter, Easing easing, float duration)
{
  mPostProcessFilter = postProcessFilter;
  mPostProcessTween.Start(easing, duration, 0.0f, 1.0f);
}

void PostProcess::ClearPostProcessFilter(Easing easing, float duration)
{
  mPostProcessTween.Start(easing, duration, mPostProcessTween.GetValue(), 0.0f);
}

void PostProcess::SetTransition(Transition transition, Easing easing, float duration)
{
  mTransition = transition;
  mTransitionTween.Start(easing, duration, 0.0f, 1.0f);
}

void PostProcess::ApplyEffect(PostProcess* postProcess, ID3D11ShaderResourceView* inSRV, ID3D11RenderTargetView* outRTV)
{
  // Set global data for full screen rendering
  Effects::PostProcessFX->SetTransition(postProcess->mPostProcessTween.GetValue());
  Effects::PostProcessFX->SetWindowSize(XMFLOAT2(gScreenH, gScreenW));
  gContext->IASetInputLayout(InputLayouts::Apla);
  gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  UINT stride = sizeof(Vertex::Apla);
  UINT offset = 0;
  D3DX11_TECHNIQUE_DESC techDesc;
  gContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
  gContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
  
  // Take inSRV and render it with a post-process effect to a mRTVs
  ID3D11ShaderResourceView* postProcessedSRV = inSRV;
  // For all techniques for a given effect
  for(UINT i=0; i<mAllFilters[postProcess->mPostProcessFilter].size(); i++) {
    ClearAndRenderTo(mBufferedRenderTargetView[i%2]);
    ID3DX11EffectTechnique* currTech = mAllFilters[postProcess->mPostProcessFilter][i];
    currTech->GetDesc(&techDesc);
    for(UINT p = 0; p < techDesc.Passes; ++p) {
      Effects::PostProcessFX->SetScreenshot(i ? mBufferedShaderResourceView[1 - i%2] : inSRV);
      currTech->GetPassByIndex(p)->Apply(0, gContext);
      gContext->DrawIndexed(6, 0, 0);
    }

    postProcessedSRV = mBufferedShaderResourceView[i%2];
  }

  // Render it back to outRTV using transition effect
  gContext->OMSetRenderTargets(1, &outRTV, gDepthStencilView);
  gContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
  ID3DX11EffectTechnique* currTech = mAllTransitions[postProcess->mTransition][0];
  Effects::TransitionFX->SetTransition(postProcess->mTransitionTween.GetValue());
  Effects::TransitionFX->SetWindowSize(XMFLOAT2(gScreenH, gScreenW));
  currTech->GetDesc(&techDesc);
  for(UINT p = 0; p < techDesc.Passes; ++p) {
    Effects::TransitionFX->SetTo(postProcessedSRV);
    currTech->GetPassByIndex(p)->Apply(0, gContext);
    gContext->DrawIndexed(6, 0, 0);
  }
  
  ClearDSV();
}