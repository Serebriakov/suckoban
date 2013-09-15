#include "LevelPause.h"
#include "Gine.h"
#include "Input.h"

void LevelPause::Enter()
{
  mPauseText.Set("PAUSE", XMFLOAT2(-200.0f, gScreenH / 2.0f - 100), TEXTALIGN_CENTER, &mPauseFont);
  mPauseText.MoveTo(XMFLOAT2(gScreenW / 2.0f, gScreenH / 2.0f - 100.0f), 0.3f, OUTX);
  mEntering = true;
}

void LevelPause::Exit()
{
  mPauseText.MoveTo(XMFLOAT2((FLOAT)gScreenW, gScreenH / 2.0f - 100), 0.3f, INX);
  mExiting = true;
}

bool LevelPause::Init()
{
  State::Init();
  return mPauseFont.Load("Lucida Console", 100, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);
}

void LevelPause::Tick(float dt)
{
  if(!mOn && mRestart) {
    Enter();
    mOn = true;
    mRestart = false;
  }

  if(mEntering && !mPauseText.IsMoving())
    mEntering = false;

  if(mExiting && !mPauseText.IsMoving()) {
    mExiting = false;
    mOn = false;
  }

  if(!mEntering && !mExiting && Input::Released('P'))
    Exit();

  mPauseText.Tick(dt);
}

void LevelPause::Draw()
{
  mPauseText.Draw();
}
