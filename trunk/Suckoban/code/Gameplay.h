#pragma once
#include "State.h"
#include "Level.h"
#include "Text.h"
#include "LevelStart.h"
#include "LevelPause.h"

/// <summary>
/// Level gameplay state.
/// Level pack + interface + level transitions.
/// </summary>

class Gameplay : public State
{
public:
  Gameplay();
  ~Gameplay() {}
  
  bool Init();
  void Tick(float dt);
  void Draw();
  
private:
  State*     mActiveSubState;
  LevelStart mLevelStartState;
  LevelPause mLevelPauseState;

  int miLevel;
  Level* mLevel;
  vector<Level> mLevels;

  bool mPaused;

  Camera mCamera;
  Font mUIFont;
  Text mUISteps;
  Text mUIPushes;
  Text mUITime;

  void UpdateUI(float dt);
  void UpdateCamera(float dt);

  void DrawUI();
};