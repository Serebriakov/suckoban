#pragma once
#include "Tween.h"
using namespace Gine;

Tween::Tween() :
  mOn(false),
  mJustFinished(false),
  mEasing(NONE),
  mTime(0.0f),
  mDuration(0.0f),
  mBeginVal(0.0f),
  mChangeVal(0.0f),
  mEndVal(0.0f),
  mCurrVal(0.0f)
{
}

bool Tween::JustFinished()
{
  if(mJustFinished) {
    mJustFinished = false;
    return true;
  }
      
  return false;
}

void Tween::Start(Easing easing, float duration, float beginVal, float endVal)
{
  mOn = true;
  mJustFinished = false;
  mEasing = easing;
  mTime = 0.0f;
  mDuration = duration;
  mBeginVal = beginVal;
  mChangeVal = endVal - beginVal;
  mEndVal = endVal;
  mCurrVal = beginVal;
}

void Tween::Tick(float dt)
{
  if(!mOn) 
    return;

  mTime += dt;
    
  if(mTime > mDuration)
  {
    mCurrVal = mEndVal;
    mTime = mDuration;
    mOn = false;
    mJustFinished = true;
  }
  else
    mCurrVal = Ease(mEasing, mTime, mDuration, mBeginVal, mChangeVal);
}

float Tween::Ease(Easing easing, float time, float duration, float begin, float change)
{
	float& t = time;
	float& b = begin;
	float& c = change;
	float& d = duration;
	
	float ts;
	float tc;

  switch(easing)
	{
	case NONE:
		t /= d;
		return b + c*t;
		
	case INX:
		ts = (t/=d)*t;
		return b + c*(ts*ts);
		
	case OUTX:
		ts = (t/=d)*t;
		tc = ts*t;
		return b+c*(-1*ts*ts + 4*tc + -6*ts + 4*t);
		
	case INOUT:
		ts=(t/=d)*t;
		tc=ts*t;
		return b+c*(6*tc*ts + -15*ts*ts + 10*tc);
		
	case OUTIN:
		ts=(t/=d)*t;
		tc=ts*t;
		return b+c*(6*tc + -9*ts + 4*t);
	
	case BACKIN:
		ts=(t/=d)*t;
		tc=ts*t;
		return b+c*(2*ts*ts + 2*tc + -3*ts);
	
	case OUTBACK:
		ts=(t/=d)*t;
		tc=ts*t;
		return b+c*(-2*ts*ts + 10*tc + -15*ts + 8*t);
	
	case OUTELASTIC:
		ts=(t/=d)*t;
		tc=ts*t;
		return b+c*(56*tc*ts + -175*ts*ts + 200*tc + -100*ts + 20*t);
	
	case INELASTIC:
		ts=(t/=d)*t;
		tc=ts*t;
		return b+c*(56*tc*ts + -105*ts*ts + 60*tc + -10*ts);

  case OUTBOUNCE:
	  if ((t/=d) < (1/2.75)) {
		  return c*(7.5625*t*t) + b;
	  } else if (t < (2/2.75)) {
		  return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
	  } else if (t < (2.5/2.75)) {
		  return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
	  } else {
		  return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
	  }

  case INBOUNCE:
    return c - Ease(Easing::OUTBOUNCE, d-t, d, 0, c) + b;

  case INOUTBOUNCE:
	  if (t < d/2) return Ease(Easing::INBOUNCE, t*2, d, 0, c) * 0.5f + b;
	  return Ease(Easing::OUTBOUNCE, t*2-d, d, 0, c) * 0.5f + c*0.5f + b;
    
  default:
    return b; 
	}
}