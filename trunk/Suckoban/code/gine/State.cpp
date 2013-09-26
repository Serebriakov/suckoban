#include "State.h"
#include "Info.h"
using namespace Gine;

State::State() : 
  Alive(false), 
  mInitialized(false),
  mPaused(false)
{
}