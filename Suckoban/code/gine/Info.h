#pragma once

#include <string>
#include "d3dUtil.h"
using std::string;
using std::wstring;

#define DISABLE_MSG false

namespace Gine
{
  class Info
  {
  public:
    static void ShowConsole();
    static void Fatal(const char* aText, ...);
    static void Log(const char* aText, ...);

    static void Print(XMFLOAT4X4& aMatrix);

  private:
    Info() { };

    static wstring StringToWString(const string s);
  };
}