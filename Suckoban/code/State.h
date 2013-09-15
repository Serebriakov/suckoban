#pragma once

/// <summary>
/// A virtual state in the game.
/// </summary>

class State
{
public:
  State() { };
  virtual ~State() = 0;
  
  virtual bool Init();
  virtual void Tick(float dt) {};
  virtual void Draw() {};

  bool IsOn()
  {
    if(!mOn) {
      mRestart = true;   
      return false;
    } else return true;
  }

protected:
  bool mOn;
  bool mEntering;
  bool mExiting;
  bool mRestart;

  virtual void Enter() { mOn = true; }
  virtual void Exit() { mOn = false; }
};