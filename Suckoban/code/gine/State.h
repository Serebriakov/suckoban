#pragma once

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

    virtual bool Init()    { return true; }
    virtual bool Destroy() { return true; }

    virtual void Tick(float dt) = 0;
    virtual void Draw() = 0;

  protected:
    bool mInitialized;
    bool mPaused;

    State();
    virtual ~State() {};
  };
}