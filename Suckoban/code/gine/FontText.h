#pragma once
#include <string>
#include "FontSheet.h"
#include "SpriteBatch.h"
using namespace std;

enum eFontStyle 
{
	FONTSTYLE_REGUAR       = 0,
	FONTSTYLE_BOLD         = 1,
	FONTSTYLE_ITALIC       = 2,
	FONTSTYLE_BOLDITALIC   = 3,
	FONTSTYLE_UNDERLINE    = 4,
	FONTSTYLE_STRIKEOUT    = 8 
};

enum eTextAlign
{
  TEXTALIGN_LEFT   = 0,
  TEXTALIGN_CENTER = 1,
  TEXTALIGN_RIGHT  = 2
};

class FontText
{
public:
  static bool Init(ID3D11Device* aDevice,
                   ID3D11DeviceContext* md3dImmediateContext);

  FontText();
  ~FontText();

  XMCOLOR color;

  bool Load(string aName, float aSize, XMCOLOR aColor, eFontStyle aStyle);
  void Print(int aX, int aY, eTextAlign aAlign, const char* aText, ...);
  void PrintAlpha(int aX, int aY, float aAlpha, eTextAlign aAlign, const char* aText, ...);

private:
	static SpriteBatch mSpriteBatch;  
  static ID3D11Device* mDevice;
  static ID3D11DeviceContext* mDeviceContext;

  FontSheet mFont;

  wstring StringToWString(const string s);
  int CalculateTextWidth(wstring aText);
};

