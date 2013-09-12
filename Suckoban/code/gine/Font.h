#pragma once
#include <string>
#include "FontSheet.h"
using namespace std;

namespace Gine 
{
  enum FontStyle 
  {
	  FONTSTYLE_REGUAR       = 0,
	  FONTSTYLE_BOLD         = 1,
	  FONTSTYLE_ITALIC       = 2,
	  FONTSTYLE_BOLDITALIC   = 3,
	  FONTSTYLE_UNDERLINE    = 4,
	  FONTSTYLE_STRIKEOUT    = 8 
  };

  /// <summary>
  /// Font with specified size, color and style
  /// </summary>

  class Font
  {
  public:
    Font();
    Font(string name, float size, XMCOLOR color, FontStyle style);
    ~Font();

    XMCOLOR Color;

    FontSheet& GetFontSheet() { return mFontSheet; };
    
    bool Load(string name, float size, XMCOLOR color, FontStyle style);
    int CalculateTextWidth(const wstring* text);

  private:
    FontSheet mFontSheet;
    bool mIsFontSheetInitialized;
  };
}