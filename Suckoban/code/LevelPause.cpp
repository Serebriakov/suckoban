#include "LevelPause.h"
#include "Gine.h"
#include "Input.h"
#include "PostProcess.h"

LevelPause LevelPause::mLevelPause;

const float TRANSITION_TIME = 0.3f;

void LevelPause::Enter()
{
  //PostProcess::SetEffect(POST_EFFECT_BLUR, Easing::OUTX, TRANSITION_TIME);
  mPauseText.Set("PAUSE", XMFLOAT2(-200.0f, gScreenH / 2.0f - 100), TEXTALIGN_CENTER, &mPauseFont);
  mPauseText.MoveTo(XMFLOAT2(gScreenW / 2.0f, gScreenH / 2.0f - 100.0f), TRANSITION_TIME, OUTX);
}

void LevelPause::Exit()
{
  //PostProcess::ClearEffect(Easing::INX, TRANSITION_TIME);
  mPauseText.MoveTo(XMFLOAT2((FLOAT)gScreenW, gScreenH / 2.0f - 100), TRANSITION_TIME, INX);
}

bool LevelPause::Init()
{
  State::Init();
  return mPauseFont.Load("Lucida Console", 100, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);
}

void LevelPause::Tick(float dt)
{
  mPauseText.Tick(dt);
}

void LevelPause::Draw()
{
  mPauseText.Draw();
}
