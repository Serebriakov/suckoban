#pragma once
#include "State.h"
#include "gine/Text.h"
#include "gine/GineUtils.h"
using namespace Gine;

const int DURATION_SEC = 4;

class LevelStart : public State
{
public:
  LevelStart();
  ~LevelStart();
  
  bool Init();
  bool Set(string aPackName, int aLevelNumber);
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

LevelStart::LevelStart()
{
}

LevelStart::~LevelStart()
{
}

bool LevelStart::Init()
{
  State::Init();
  mPackFont.Load("Lucida Console", 100, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);
  mLevelFont.Load("Lucida Console", 60, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);
  mCountdownFont.Load("Lucida Console", 80, XMCOLOR(0xffeeeeec),
                      FONTSTYLE_REGUAR);

  return true;
}

bool LevelStart::Set(string aPackName, int aLevelNumber)
{
  mTimer = 0.0f;

  int packTextY = gScreenH / 2 - 140;
  int levelTextY = gScreenH / 2;
  int countdownTextY = gScreenH / 2 + 80;

  mPackText.Set(aPackName, XMFLOAT2(gScreenW / 2, packTextY), TEXTALIGN_CENTER, &mPackFont);
  mLevelText.Set("LEVEL " + Gine::Utils::ToString(aLevelNumber), XMFLOAT2(gScreenW / 2, levelTextY), TEXTALIGN_CENTER,
                 &mLevelFont);
  mCountdownText.Set(Gine::Utils::ToString(DURATION_SEC - 1),  XMFLOAT2(gScreenW / 2, countdownTextY),
                     TEXTALIGN_CENTER, &mCountdownFont);

  return true;
}

void LevelStart::Tick(float dt)
{
  mOn = true;

  int oldTime = (int)mTimer;
  mTimer += dt;
  int time = (int)mTimer;
  mCountdownText.Value = Gine::Utils::ToString(DURATION_SEC - 1 - time);

  if(time == DURATION_SEC - 1)
  {
    if(oldTime == DURATION_SEC - 2)
    {
      mPackText.MoveTo(XMFLOAT2(mPackText.Position.x, -100), 1.0f, Gine::Easing::BACKIN);
      mLevelText.MoveTo(XMFLOAT2(mLevelText.Position.x, -100), 1.0f, Gine::Easing::BACKIN);
      mCountdownText.MoveTo(XMFLOAT2(mCountdownText.Position.x, -100), 1.0f, Gine::Easing::BACKIN);
    }
  }

  if(time >= DURATION_SEC)
    mOn = false;

  mPackText.Tick(dt);
  mLevelText.Tick(dt);
  mCountdownText.Tick(dt);
}

void LevelStart::Draw()
{
  mPackText.Draw();
  mLevelText.Draw();
  mCountdownText.Draw();
}
