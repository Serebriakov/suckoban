#pragma once
#include "Gine.h"
#include "PostProcess.h"

/// <summary>
/// A virtual state in the game.
/// </summary>

namespace Gine
{
  class State
  {
    friend class StateMachine;

  public:
    /// <summary> Can the state be removed from the state machine's stack? </summary>
    bool Alive;
  
    virtual void Enter() { Alive = true; }
    virtual void Exit()  { Alive = false; }

    virtual void Pause()  { mPaused = true; }
    virtual void Resume() { mPaused = false; }

    virtual bool Init();
    virtual bool Destroy();

    virtual void Tick(float dt);
    virtual void Draw() = 0;

    ID3D11ShaderResourceView* GetSRV() { return mSRV; }
    ID3D11RenderTargetView*   GetRTV() { return mRTV; }
    PostProcess* GetPostProcess() { return &mPostProcess; }

  protected:
    bool mInitialized;
    bool mPaused;
    float mShutdown;
    
    ID3D11ShaderResourceView* mSRV;
    ID3D11RenderTargetView*   mRTV;

    PostProcess mPostProcess;

    State();
    virtual ~State() {};

    void KillAfter(float time) { mShutdown = time; }
  };
}