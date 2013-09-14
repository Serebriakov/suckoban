#include "Font.h"
#include "GineUtils.h"
using namespace Gine;

int Font::CalculateTextWidth(const wstring* text) const
{
  if(!mIsFontSheetInitialized) 
  {
    Info::Fatal("FontSheet not initialized");
    return 0;
  }

  int textWidth = 0;
	for(UINT i = 0; i < text->size(); ++i)
	{
		WCHAR character = text->at(i);
		if(character == ' ') 
		{
			textWidth += mFontSheet.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = mFontSheet.GetCharRect(text->at(i));
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

Font::Font(const char* name, float size, XMCOLOR color, FontStyle style)
{
  mIsFontSheetInitialized = false;
  Load(name, size, color, style);
}

Font::~Font()
{
}

bool Font::Load(const char* name, float size, XMCOLOR color, FontStyle style)
{  
  if(mIsFontSheetInitialized)
  {
    Info::Fatal("FontSheet for that font already initialized");
    return false;
  }

  HRESULT result = mFontSheet.Initialize(gDevice, Utils::ToWString(&string(name)), size, (FontSheet::FontStyle) style, true);
  if(FAILED(result))
  {
    Info::Fatal("FontSheet initialization failed with error code 0x%08lx", result);
    return false;
  }

  Color = color;
  mIsFontSheetInitialized = true;

  return true;
}