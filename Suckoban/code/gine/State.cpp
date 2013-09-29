#include "State.h"
#include "Info.h"
#include "GineUtils.h"
using namespace Gine;

State::State() : 
  Alive(false), 
  mInitialized(false),
  mPaused(false),
  mSRV(0),
  mRTV(0),
  mShutdown(-1.0f)
{
}

bool State::Init()
{
  ID3D11Texture2D* texture = Utils::CreateDefaultTexture2D();
  
  HR(gDevice->CreateShaderResourceView(texture, 0, &mSRV));
	HR(gDevice->CreateRenderTargetView  (texture, 0, &mRTV));
  
  ReleaseCOM(texture);

  return true;
}

bool State::Destroy()
{
  ReleaseCOM(mSRV);
  ReleaseCOM(mRTV);

  return true;
}

void State::Tick(float dt)
{
  mPostProcess.Tick(dt);

  if(mShutdown > 0.0f) {
    mShutdown -= dt;
    if(mShutdown <= 0.0f) {
      Alive = false;
      mShutdown = -1.0f;
    }
  }
}