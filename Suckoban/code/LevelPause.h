#pragma once
#include "State.h"
#include "gine/Text.h"
#include "gine/GineUtils.h"
#include "gine/Input.h"
using namespace Gine;

class LevelPause : public State
{
public:
  LevelPause();
  ~LevelPause();
  
  bool Init();
  void Tick(float dt);
  void Draw();

  void Enter();
  void Exit();
  bool IsActive();
  
private:
  Font mPauseFont;
  Text mPauseText;
};

LevelPause::LevelPause()
{
}

LevelPause::~LevelPause()
{
}

void LevelPause::Enter()
{
  mPauseText.Set("PAUSE", XMFLOAT2(-200, gScreenH / 2 - 100), TEXTALIGN_CENTER, &mPauseFont);
  mPauseText.MoveTo(XMFLOAT2(gScreenW / 2, gScreenH / 2 - 100), 0.3f, Easing::OUTX);
  mEntering = true;
}

void LevelPause::Exit()
{
  mPauseText.MoveTo(XMFLOAT2(gScreenW, gScreenH / 2 - 100), 0.3f, Easing::INX);
  mExiting = true;
}

bool LevelPause::Init()
{
  State::Init();
  mPauseFont.Load("Lucida Console", 100, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);

  return true;
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

  if(mExiting && !mPauseText.IsMoving())
  {
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
