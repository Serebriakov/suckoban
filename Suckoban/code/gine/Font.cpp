#include "Font.h"
#include "Gine.h"
#include "GineUtils.h"
using namespace Gine;

int Font::CalculateTextWidth(const wstring* aText)
{
  if(!mIsFontSheetInitialized) 
  {
    Info::Fatal("Trying to calculate width of an undefined text");
    return 0;
  }

  int textWidth = 0;
	for(UINT i = 0; i < aText->size(); ++i)
	{
		WCHAR character = aText->at(i);
		if(character == ' ') 
		{
			textWidth += mFontSheet.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = mFontSheet.GetCharRect(aText->at(i));
			textWidth += (r.right - r.left + 1);
		}
	}

  return textWidth;
}

Font::Font()
{
  mIsFontSheetInitialized = false;
  Color = XMCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
}

Font::Font(string aName, float aSize, XMCOLOR aColor, eFontStyle aStyle)
{
  mIsFontSheetInitialized = false;
  Load(aName, aSize, aColor, aStyle);
}

Font::~Font()
{
}

bool Font::Load(string aName, float aSize, XMCOLOR aColor, eFontStyle aStyle)
{  
  if(mIsFontSheetInitialized)
  {
    Info::Fatal("FontSheet for that font already initialized");
    return false;
  }

  HRESULT result = mFontSheet.Initialize(gDevice, Utils::ToWString(&aName), aSize, (FontSheet::FontStyle) aStyle, true);
  if(FAILED(result))
  {
    Info::Fatal("FontSheet initialization failed with error code 0x%08lx", result);
    return false;
  }

  Color = aColor;
  mIsFontSheetInitialized = true;

  return true;
}