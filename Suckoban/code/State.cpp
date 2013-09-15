#include "State.h"

State::~State()
{
}

bool State::Init()
{
  mOn = false;
  mEntering = false;
  mExiting = false;
  mRestart = true;

  return true;
}