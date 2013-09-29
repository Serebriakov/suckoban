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
  static Gameplay* GetInstance() { return &mGameplay; }

  void Pause();
  void Resume();

  bool Init();
  bool Destroy() { return State::Destroy(); }

  void Tick(float dt);
  void Draw();
  
private:
  static Gameplay mGameplay;

  int miLevel;
  Level* mLevel;
  vector<Level> mLevels;

  Camera mCamera;
  Font mUIFont;
  Text mUISteps;
  Text mUIPushes;
  Text mUITime;

  Gameplay();
  ~Gameplay() {}

  void UpdateUI(float dt);
  void UpdateCamera(float dt);

  void DrawUI();
};