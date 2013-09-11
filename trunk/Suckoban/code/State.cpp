#include "State.h"

bool State::Init()
{
  mOn = false;
  mEntering = false;
  mExiting = false;
  mRestart = true;

  return true;
}