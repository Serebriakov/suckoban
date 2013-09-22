#include "PostProcess.h"
#include "GeometryGenerator.h"
#include "Effects.h"
using namespace Gine;

map<PostEffect, vector<ID3DX11EffectTechnique*>> PostProcess::mAllEfects;
ID3D11RenderTargetView*   PostProcess::mBufferedRenderTargetView[2];
ID3D11ShaderResourceView* PostProcess::mBufferedShaderResourceView[2];
ID3D11Buffer*             PostProcess::mScreenQuadVB = 0;
ID3D11Buffer*             PostProcess::mScreenQuadIB = 0;
Tween                     PostProcess::mTransition;
PostEffect                PostProcess::mCurrEffect = POST_EFFECT_NONE;

bool PostProcess::Init()
{
  CreateBackBufferViews();
  CreateBackBufferBuffers();

  vector<ID3DX11EffectTechnique*> effectTechs;

  effectTechs.push_back(Effects::PostProcessFX->DefaultTech);
  mAllEfects[POST_EFFECT_NONE] = effectTechs;

  effectTechs.clear();
  effectTechs.push_back(Effects::PostProcessFX->GreyscaleTech);
  mAllEfects[POST_EFFECT_GREYSCALE] = effectTechs;

  effectTechs.clear();
  effectTechs.push_back(Effects::PostProcessFX->BlurHTech);
  effectTechs.push_back(Effects::PostProcessFX->BlurVTech);
  mAllEfects[POST_EFFECT_BLUR] = effectTechs;

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
  if(mCurrEffect != POST_EFFECT_NONE) {
    mTransition.Tick(dt);

    // If transition is finished and effects cleared - set technique to default
    // GetValue have to return 0 right now, but we are not comparing floats of course
    if(!mTransition.IsOn() && mTransition.GetValue() < 0.1f)
      mCurrEffect = POST_EFFECT_NONE;
  }
}

void PostProcess::CreateBackBufferViews()
{
  // Texture description
	D3D11_TEXTURE2D_DESC texDesc;
  texDesc.Width              = gScreenW;
  texDesc.Height             = gScreenH;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.SampleDesc.Count   = 1;
  texDesc.SampleDesc.Quality = 0;
	texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0; 
	texDesc.MiscFlags          = 0;

  // Texture and views
	ID3D11Texture2D* texture0 = 0;
	HR(gDevice->CreateTexture2D         (&texDesc, 0, &texture0));
  HR(gDevice->CreateShaderResourceView(texture0,  0, &mBufferedShaderResourceView[0]));
	HR(gDevice->CreateRenderTargetView  (texture0,  0, &mBufferedRenderTargetView[0]));
  ID3D11Texture2D* texture1 = 0;
	HR(gDevice->CreateTexture2D         (&texDesc, 0, &texture1));
  HR(gDevice->CreateShaderResourceView(texture1,  0, &mBufferedShaderResourceView[1]));
	HR(gDevice->CreateRenderTargetView  (texture1,  0, &mBufferedRenderTargetView[1]));

	// Release
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

void PostProcess::RenderToBackBuffer()
{
  gContext->OMSetRenderTargets(1, &mBufferedRenderTargetView[0], gDepthStencilView);
	gContext->ClearRenderTargetView(mBufferedRenderTargetView[0], reinterpret_cast<const float*>(&Colors::Black));
  gContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void PostProcess::RenderToScreen()
{
  gContext->OMSetRenderTargets(1, &gRenderTargetView, gDepthStencilView);
  gContext->ClearRenderTargetView(gRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
  gContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void PostProcess::SwapRTVBuffers()
{
  ID3D11RenderTargetView* RTV = mBufferedRenderTargetView[0];
  mBufferedRenderTargetView[0] = mBufferedRenderTargetView[1];
  mBufferedRenderTargetView[1] = RTV;     
}

void PostProcess::SwapSRVBuffers()
{
  ID3D11ShaderResourceView* SRV = mBufferedShaderResourceView[0];
  mBufferedShaderResourceView[0] = mBufferedShaderResourceView[1];
  mBufferedShaderResourceView[1] = SRV;      
}

void PostProcess::SetEffect(PostEffect postEffect, Easing easing, float duration)
{
  mCurrEffect = postEffect;
  mTransition.Start(easing, duration, 0.0f, 1.0f);
}

void PostProcess::ClearEffect(Easing easing, float duration)
{
  mTransition.Start(easing, duration, 1.0f, 0.0f);
}

void PostProcess::DrawBackBuffer()
{
  Effects::PostProcessFX->SetTransition(mTransition.GetValue());
  Effects::PostProcessFX->SetWindowSize(XMFLOAT2(gScreenH, gScreenW));

  // For all techniques in current effect
  for(UINT i=0; i<mAllEfects[mCurrEffect].size(); i++) {

    if(i == mAllEfects[mCurrEffect].size() - 1)
      RenderToScreen();
    else {
      SwapRTVBuffers();
      RenderToBackBuffer();
    }

    gContext->IASetInputLayout(InputLayouts::Apla);
    gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
    UINT stride = sizeof(Vertex::Apla);
    UINT offset = 0;
 
    D3DX11_TECHNIQUE_DESC techDesc;
    mAllEfects[mCurrEffect][i]->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p) {
      gContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
      gContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
      Effects::PostProcessFX->SetScreenshot(mBufferedShaderResourceView[0]);
      mAllEfects[mCurrEffect][i]->GetPassByIndex(p)->Apply(0, gContext);
      gContext->DrawIndexed(6, 0, 0);
    }

    if(i != mAllEfects[mCurrEffect].size() - 1)
      SwapSRVBuffers();
  }

  ClearDSV();
}