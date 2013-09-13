#pragma once
#include "d3dUtil.h"

/// <summary> Disable message boxes from showing up </summary>
const bool DISABLE_MSG = false;

namespace Gine
{
  /// <summary>
  /// Shows logs in console and fatal errors in message boxes. 
  /// TODO: Print logs to a file
  ///       Fix before publishing Suckoban
  /// </summary>

  class Info
  {
  public:
    static void ShowConsole();
    static void Fatal(const char* aText, ...);
    static void Log(const char* aText, ...);

    static void Print(XMFLOAT4X4& aMatrix);

  private:
    Info() { };
  };
}