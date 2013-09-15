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
  LevelPause() {}
  ~LevelPause() {}
  
  bool Init();
  void Tick(float dt);
  void Draw();

  void Enter();
  void Exit();
  
private:
  Font mPauseFont;
  Text mPauseText;
};