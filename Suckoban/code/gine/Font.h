#pragma once
#include <string>
#include "FontSheet.h"
#include "SpriteBatch.h"
using namespace std;

namespace Gine 
{
  enum eFontStyle 
  {
	  FONTSTYLE_REGUAR       = 0,
	  FONTSTYLE_BOLD         = 1,
	  FONTSTYLE_ITALIC       = 2,
	  FONTSTYLE_BOLDITALIC   = 3,
	  FONTSTYLE_UNDERLINE    = 4,
	  FONTSTYLE_STRIKEOUT    = 8 
  };

  class Font
  {
  public:
    Font();
    Font(string aName, float aSize, XMCOLOR aColor, eFontStyle aStyle);
    ~Font();

    XMCOLOR Color;
    
    int CalculateTextWidth(const wstring* aText);
    FontSheet& GetFontSheet() { return mFontSheet; };
    bool Load(string aName, float aSize, XMCOLOR aColor, eFontStyle aStyle);

  private:
    FontSheet mFontSheet;
    bool mIsFontSheetInitialized;
  };
}