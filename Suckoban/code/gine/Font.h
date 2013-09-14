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
    Font(const char* name, float size, XMCOLOR color, FontStyle style);
    ~Font();

    XMCOLOR Color;

    FontSheet& GetFontSheet() { return mFontSheet; };
    
    bool Load(const char* name, float size, XMCOLOR color, FontStyle style);

    /// <summary> Calculates the text width </summary>
    /// <param name="text"> The text to be calculated </param>
    /// <returns> The calculated text width in pixels </returns>
    int CalculateTextWidth(const wstring* text) const;

  private:
    FontSheet mFontSheet;
    bool mIsFontSheetInitialized;
  };
}