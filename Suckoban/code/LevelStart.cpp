#include "LevelStart.h"
#include "GineUtils.h"

bool LevelStart::Init()
{
  State::Init();
  mPackFont.Load     ("Lucida Console", 100, XMCOLOR(0x88eeeeec), FONTSTYLE_REGUAR);
  mLevelFont.Load    ("Lucida Console", 60,  XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);
  mCountdownFont.Load("Lucida Console", 80,  XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);

  return true;
}

bool LevelStart::Set(string aPackName, int aLevelNumber)
{
  mTimer = 0.0f;

  float packTextY = gScreenH / 2.0f - 140.0f;
  float levelTextY = gScreenH / 2.0f;
  float countdownTextY = gScreenH / 2.0f + 80.0f;

  mPackText.Set(aPackName.c_str(), XMFLOAT2(gScreenW / 2.0f, packTextY), TEXTALIGN_CENTER, &mPackFont);

  string nLevel = "LEVEL " + Gine::Utils::ToString(aLevelNumber);
  mLevelText.Set(nLevel.c_str(), XMFLOAT2(gScreenW / 2.0f, levelTextY), TEXTALIGN_CENTER, &mLevelFont);

  string countdown = Gine::Utils::ToString(COUNTDOWN);
  mCountdownText.Set(countdown.c_str(),  XMFLOAT2(gScreenW / 2.0f, countdownTextY), TEXTALIGN_CENTER, &mCountdownFont);

  return true;
}

void LevelStart::Tick(float dt)
{
  mOn = true;

  int oldTime = (int)mTimer;
  mTimer += dt;
  int time = (int)mTimer;
  mCountdownText.Value = Gine::Utils::ToString(COUNTDOWN - time);

  if(time == COUNTDOWN)
  {
    if(oldTime == COUNTDOWN - 1)
    {
      mPackText.MoveTo(XMFLOAT2(mPackText.Position.x, -100), 1.0f, BACKIN);
      mLevelText.MoveTo(XMFLOAT2(mLevelText.Position.x, -100), 1.0f, BACKIN);
      mCountdownText.MoveTo(XMFLOAT2(mCountdownText.Position.x, -100), 1.0f, BACKIN);
    }
  }

  if(time >= COUNTDOWN + 1)
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
