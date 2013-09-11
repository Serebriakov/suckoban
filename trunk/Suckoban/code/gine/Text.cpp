#include "Text.h"
#include "Gine.h"
#include "GineUtils.h"
using namespace Gine;

SpriteBatch Text::mSpriteBatch;
bool Text::mIsSpriteBatchInitialized = false;

bool Text::InitSpriteBatch()
{
  HRESULT result = mSpriteBatch.Initialize(Gine::gDevice);
  if(FAILED(result))
  {
    Info::Fatal("SpriteBatch initialization failed with error code 0x%08lx", result);
    return false;
  }

  mIsSpriteBatchInitialized = true;
  return true;
}

Text::Text()
{
}

Text::Text(string aText, XMFLOAT2 aPosition, eTextAlign aAlign, Font* aFont)
{
  Set(aText, aPosition, aAlign, aFont);
}

Text::~Text()
{
}

void Text::MoveTo(XMFLOAT2 aDestination, float aDuration, Easing::Enum aEasing)
{
  mTextXTween.Start(aEasing, aDuration, Position.x, aDestination.x, true);
  mTextYTween.Start(aEasing, aDuration, Position.y, aDestination.y, true);
}

void Text::MoveBy(XMFLOAT2 aDistance, float aDuration, Easing::Enum aEasing)
{
  mTextXTween.Start(aEasing, aDuration, Position.x, Position.x + aDistance.x, true);
  mTextYTween.Start(aEasing, aDuration, Position.y, Position.y + aDistance.y, true);
}

void Text::Set(string aText, XMFLOAT2 aPosition, eTextAlign aAlign, Font* aFont)
{  
  Value = aText;
  Position = aPosition;
  Align = aAlign;
  DisplayFont = aFont;
}

void Text::Tick(float dt)
{
  if(mTextXTween.IsOn() || mTextYTween.IsOn()) {
    Position.x = mTextXTween.GetValue();
    Position.y = mTextYTween.GetValue();
    mTextXTween.Tick(dt);
    mTextYTween.Tick(dt);
  }
}

void Text::Draw()
{
  if(Value.empty())
    return;

  if(DisplayFont == 0) 
  {
    Info::Fatal("Trying to draw a text with undefined font");
    return;
  }

	wstring wText = Utils::ToWString(&Value);
  POINT textPos = {(long)Position.x, (long)Position.y};

  if(mCachedText != Value) 
  {
    mCachedTextWidth = DisplayFont->CalculateTextWidth(&wText);
    mCachedText = Value;
  }

  if(TEXTALIGN_RIGHT == Align)
  {
    textPos.x -= mCachedTextWidth;
  }
  else if(TEXTALIGN_CENTER == Align)
  {
    textPos.x -= mCachedTextWidth / 2;
  }

  //XMCOLOR newCol = color;
  //newCol.a = aAlpha * 255;

  if(!mIsSpriteBatchInitialized)
    InitSpriteBatch();

  mSpriteBatch.DrawString(gContext, DisplayFont->GetFontSheet(), wText, textPos, DisplayFont->Color);
}