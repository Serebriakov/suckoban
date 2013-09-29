#include "LevelPause.h"
#include "Gine.h"
#include "Input.h"
#include "PostProcess.h"
#include "StateMachine.h"

LevelPause LevelPause::mLevelPause;

const float TRANSITION_TIME = 0.3f;

void LevelPause::Enter()
{
  Alive = true;
  mPostProcess.SetTransition(TRANSITION_FADEIN, OUTX, TRANSITION_TIME);
}

void LevelPause::Exit()
{
  mPostProcess.SetTransition(TRANSITION_FADEOUT, INX, TRANSITION_TIME);
  KillAfter(TRANSITION_TIME);
}

bool LevelPause::Init()
{
  State::Init();
  if(!mPauseFont.Load("Lucida Console", 100, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR))
    Info::Fatal("LevelPause init failed");
  
  mPauseText.Set("PAUSE", XMFLOAT2(gScreenW / 2.0f, gScreenH / 2.0f - 100), TEXTALIGN_CENTER, &mPauseFont);

  mInitialized = true;
  return true;
}

void LevelPause::Tick(float dt)
{
  State::Tick(dt);

  if(Alive && mShutdown < 0.0f) {
    if(Input::Released('P'))
      StateMachine::Pop();
  }
}

void LevelPause::Draw()
{
  mPauseText.Draw();
}
