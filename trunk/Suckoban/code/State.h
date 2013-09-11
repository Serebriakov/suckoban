#pragma once

class State
{
public:
  State() { };
  virtual ~State() { };
  
  virtual bool Init() = 0;
  virtual void Tick(float dt) = 0;
  virtual void Draw() = 0;

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