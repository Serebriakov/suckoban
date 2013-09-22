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
  LevelStart() {}
  ~LevelStart() {}
  
  bool Init();
  bool Set(string packName, int levelNumber);
  void Tick(float dt);
  void Draw();
  
private:
  float mTimer;

  Font mPackFont;
  Font mLevelFont;
  Font mCountdownFont;

  Text mPackText;
  Text mLevelText;
  Text mCountdownText;
};