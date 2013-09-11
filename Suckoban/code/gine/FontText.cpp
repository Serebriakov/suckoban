#include "FontText.h"

SpriteBatch FontText::mSpriteBatch;
ID3D11Device* FontText::mDevice = 0;
ID3D11DeviceContext* FontText::mDeviceContext = 0;

bool FontText::Init(ID3D11Device* aDevice,
                    ID3D11DeviceContext* aDeviceContext)
{
  mDevice = aDevice;
  mDeviceContext = aDeviceContext;
  mSpriteBatch.Initialize(aDevice);

  return true;
}

wstring FontText::StringToWString(const string s)
{
  wstring wsTmp(s.begin(), s.end());
  return wsTmp;
}

int FontText::CalculateTextWidth(wstring aText)
{
  int textWidth = 0;
	for(UINT i = 0; i < aText.size(); ++i)
	{
		WCHAR character = aText[i];
		if(character == ' ') 
		{
			textWidth += mFont.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = mFont.GetCharRect(aText[i]);
			textWidth += (r.right - r.left + 1);
		}
	}

  return textWidth;
}

FontText::FontText()
{
}

FontText::~FontText()
{
}

bool FontText::Load(string aName, float aSize, XMCOLOR aColor, 
                    eFontStyle aStyle)
{  
  HRESULT result = mFont.Initialize(mDevice, StringToWString(aName), aSize,
                                    (FontSheet::FontStyle) aStyle, true);
  if(FAILED(result))
  {
    return false;
  }

  color = aColor;

  return true;
}

void FontText::Print(int aX, int aY, eTextAlign aAlign, const char* aText, ...)
{
  char buff[1024];
  va_list args;
  va_start(args, aText);
  vsprintf(buff, aText, args);
  va_end(args);

	wstring wText = StringToWString(buff);

	POINT textPos;
	textPos.x = aX;
	textPos.y = aY;

  if(TEXTALIGN_RIGHT == aAlign)
  {
    int w = CalculateTextWidth(wText);
    textPos.x -= w;
  }
  else if(TEXTALIGN_CENTER == aAlign)
  {
    int w = CalculateTextWidth(wText);
    textPos.x -= w / 2;
  }

  mSpriteBatch.DrawString(mDeviceContext, mFont, wText, textPos, color);
}

void FontText::PrintAlpha(int aX, int aY, float aAlpha, eTextAlign aAlign,
                          const char* aText, ...)
{
  char buff[1024];
  va_list args;
  va_start(args, aText);
  vsprintf(buff, aText, args);
  va_end(args);

	wstring wText = StringToWString(buff);

	POINT textPos;
	textPos.x = aX;
	textPos.y = aY;

  if(TEXTALIGN_RIGHT == aAlign)
  {
    int w = CalculateTextWidth(wText);
    textPos.x -= w;
  }
  else if(TEXTALIGN_CENTER == aAlign)
  {
    int w = CalculateTextWidth(wText);
    textPos.x -= w / 2;
  }

  XMCOLOR newCol = color;
  newCol.a = aAlpha * 255;

  mSpriteBatch.DrawString(mDeviceContext, mFont, wText, textPos, newCol);
}