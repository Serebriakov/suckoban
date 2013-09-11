#pragma once

namespace Gine
{
  namespace Easing { enum Enum
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
  };}

  class Tween
  {
  public:
    Tween();
          
    bool  IsOn()     { return mOn; };
    float GetTime()  { return mTime; };
    float GetValue() { return mCurrVal; };
    bool  JustFinished();
    
    void Start(Easing::Enum aType, float aDuration, float aBeginVal,
               float aEndVal, bool aStarted = false);
    void Stop();
    void Tick(float dt);  
    
  private:
    bool mOn;
    bool mJustFinished;
    
    Easing::Enum mType;
    float mTime;
    float mDuration;
    
    float mBeginVal;
    float mChangeVal;
    float mEndVal;
    float mCurrVal;
    
    float Ease(Easing::Enum aType, float aTime, float aDuration, 
               float aBegin, float aChange);
  };
}