#pragma once
#include "State.h"
#include "Text.h"
using namespace Gine;

/// <summary>
/// Paused game state.
/// </summary>

class LevelPause : public State
{
public:
  static LevelPause* GetInstance() { return &mLevelPause; }

  bool Init();
  bool Destroy() { return true; }

  void Enter();
  void Exit();

  void Tick(float dt);
  void Draw();

private:
  static LevelPause mLevelPause;

  Font mPauseFont;
  Text mPauseText;
  
  LevelPause() : State() {}
  ~LevelPause() {}
};