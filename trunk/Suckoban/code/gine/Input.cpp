#include "Input.h"
using namespace Gine;

bool Input::mLastFrame[N_VIRTUAL_KEYS];
bool Input::mThisFrame[N_VIRTUAL_KEYS];

void Input::RefreshKeys(bool* aFrame)
{
  for(int i=0; i<N_VIRTUAL_KEYS; i++)
    aFrame[i] = GetAsyncKeyState(i) & 0x8000;
}

bool Input::Init()
{
  RefreshKeys(mLastFrame);
  RefreshKeys(mThisFrame);

  return true;
}

void Input::Tick(float dt)
{
  for(int i=0; i<N_VIRTUAL_KEYS; i++)
    mLastFrame[i] = mThisFrame[i];

  RefreshKeys(mThisFrame);
}

bool Input::Released(int aKey)
{
  return !mThisFrame[aKey] && mLastFrame[aKey];
}

bool Input::PressedDir(Dir::Enum& aDir)
{
  if((GetAsyncKeyState('W') & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000))
    aDir = Dir::UP;
  else if((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000))
    aDir = Dir::RIGHT;
  else if((GetAsyncKeyState('S') & 0x8000) || (GetAsyncKeyState(VK_DOWN) & 0x8000))
    aDir = Dir::DOWN;
  else if((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000))
    aDir = Dir::LEFT;
  else return false;

  return true;
}