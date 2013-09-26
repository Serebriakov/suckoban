#pragma once
#include "State.h"
#include "Text.h"
using namespace Gine;

/// <summary>
/// Countdown at the begining of the level with level's number and pack's name.
/// </summary>

class LevelStart : public State
{
public:  
  static LevelStart* GetInstance() { return &mLevelStart; }

  bool Init();
  bool Destroy() { return true; }

  bool Set(string packName, int levelNumber);

  void Tick(float dt);
  void Draw();

private:
  static LevelStart mLevelStart;

  float mTimer;

  Font mPackFont;
  Font mLevelFont;
  Font mCountdownFont;

  Text mPackText;
  Text mLevelText;
  Text mCountdownText;

  LevelStart() : State() {}
  ~LevelStart() {}
};