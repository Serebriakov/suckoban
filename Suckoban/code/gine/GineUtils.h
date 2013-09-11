#pragma once
#include <string>
#include "Gine.h"
using namespace std;

namespace Gine
{
  namespace Utils 
  { 
    wstring ToWString(const string* s);  
    string ToString(const int i);
    string Format(const char* text, ...);

    string VFormat(const char* text, va_list ap);
  }
}