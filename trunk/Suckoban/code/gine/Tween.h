#pragma once

namespace Gine
{
  enum Easing
  {
    NONE = 0,
    INX,
    OUTX,
    INOUT, 
    OUTIN,
    BACKIN,
    OUTBACK,
    OUTELASTIC,
    INELASTIC,
    INBOUNCE,
    OUTBOUNCE,
    INOUTBOUNCE
  };

  /// <summary>
  /// Controls smooth transitions between two float values.
  /// </summary>

  class Tween
  {
  public:
    Tween();
          
    bool  IsOn()     const { return mOn; }
    float GetTime()  const { return mTime; }
    float GetValue() const { return mCurrVal; }
    bool  JustFinished();
    
    void Start(Easing easing, float duration, float beginVal, float endVal);
    void Stop() { mOn = false; }
    void Tick(float dt);  
    
  private:
    bool mOn;
    bool mJustFinished;
    
    Easing mEasing;
    float mTime;
    float mDuration;
    
    float mBeginVal;
    float mChangeVal;
    float mEndVal;
    float mCurrVal;
    
    float Ease(Easing easing, float time, float duration, float begin, float change);
  };
}