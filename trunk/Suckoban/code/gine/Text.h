#pragma once
#include <string>
#include "SpriteBatch.h"
#include "Font.h"
#include "Tween.h"
using namespace std;

namespace Gine
{
  enum eTextAlign
  {
    TEXTALIGN_LEFT   = 0,
    TEXTALIGN_CENTER = 1,
    TEXTALIGN_RIGHT  = 2
  };

  class Text
  {
  public:
    Text();
    Text(string aText, XMFLOAT2 aPosition, eTextAlign aAlign, Font* aFont);
    ~Text();

    string Value;
    XMFLOAT2 Position;
    eTextAlign Align;
    Font* DisplayFont;

    void MoveTo(XMFLOAT2 aDestination, float aDuration, Easing::Enum aEasing);
    void MoveBy(XMFLOAT2 aDistance, float aDuration, Easing::Enum aEasing);
    bool IsMoving() { return mTextXTween.IsOn() || mTextYTween.IsOn(); }

    void Set(string aText, XMFLOAT2 aPosition, eTextAlign aAlign, Font* aFont);
    void Tick(float dt);
    void Draw();

  private:
	  static SpriteBatch mSpriteBatch;  
    static bool mIsSpriteBatchInitialized;

    string mCachedText;
    int mCachedTextWidth;
    Tween mTextXTween;
    Tween mTextYTween;
    
    static bool InitSpriteBatch();
  };
}

