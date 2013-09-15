#pragma once
#include "GineUtils.h";

/// <summary> Number of default Windows virtual keys </summary>
const int N_VIRTUAL_KEYS = 0xFE;

namespace Gine
{
  /// <summary>
  /// Captures all user input 
  /// TODO: Mouse capture
  ///       Xbox 360 Controller capture
  /// </summary>

  class Input
  {
  public:
    static bool Init();
    static void Tick(float dt);
    
    static bool Pressed (int aKey);
    static bool Down    (int aKey);
    static bool Released(int aKey);
    static bool PressedDir(Dir::Enum& aDir);

  private:
    static bool mLastFrame[N_VIRTUAL_KEYS];
    static bool mThisFrame[N_VIRTUAL_KEYS];

    static void RefreshKeys(bool* aFrame);

    Input() { };
  };
}