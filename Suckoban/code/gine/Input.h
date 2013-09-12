#pragma once
#include "GineUtils.h"

const int N_VIRTUAL_KEYS = 0xFE;

namespace Gine
{
  class Input
  {
  public:
    static bool Init();
    static void Tick(float dt);

    static bool Released(int aKey);
    static bool PressedDir(Dir::Enum& aDir);


  private:
    static bool mLastFrame[N_VIRTUAL_KEYS];
    static bool mThisFrame[N_VIRTUAL_KEYS];

    static void RefreshKeys(bool* aFrame);

    Input() { };
  };
}