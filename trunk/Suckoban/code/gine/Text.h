#pragma once
#include <string>
#include "SpriteBatch.h"
#include "Font.h"
#include "Tween.h"
using namespace std;

namespace Gine
{
  enum TextAlign
  {
    TEXTALIGN_LEFT   = 0,
    TEXTALIGN_CENTER = 1,
    TEXTALIGN_RIGHT  = 2
  };

  /// <summary>
  /// A single text displayed on screen
  /// </summary>

  class Text
  {
  public:
    Text();
    Text(const char* text, XMFLOAT2 position, TextAlign align, Font* font);
    ~Text();

    string      Value;
    XMFLOAT2    Position;
    TextAlign   Align;
    Font*       DisplayFont;

    void MoveTo(XMFLOAT2 aDestination, float aDuration, Easing aEasing);
    void MoveBy(XMFLOAT2 aDistance,    float aDuration, Easing aEasing);
    bool IsMoving() const { return mTextXTween.IsOn() || mTextYTween.IsOn(); }

    void Set(const char* text, XMFLOAT2 position, TextAlign align, Font* font);
    void Tick(float dt);
    void Draw();

  private:
	  static SpriteBatch mSpriteBatch;  
    static bool mSpriteBatchInitialized;

    string mCachedText;
    int    mCachedTextWidth;
    Tween  mTextXTween;
    Tween  mTextYTween;
    
    static bool InitSpriteBatch();
  };
}

