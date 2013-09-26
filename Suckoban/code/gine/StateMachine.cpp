#include "StateMachine.h"
using namespace Gine;

vector<State*> StateMachine::mStates;
vector<State*> StateMachine::mAllStates;
Tween          StateMachine::mTransitionTween;
Transition     StateMachine::mCurrTransition = TRANSITION_NONE;

void StateMachine::Change(State* state, Transition transition, float duration, Easing easing)
{
	if(!mStates.empty())
		mStates.back()->Exit();

	mStates.push_back(state);
	mStates.back()->Enter();

  RememberState(state);
}

void StateMachine::Push(State* state, Transition transition, float duration, Easing easing)
{
	if(!mStates.empty())
		mStates.back()->Pause();

	mStates.push_back(state);
	mStates.back()->Enter();

  RememberState(state);
}

void StateMachine::Pop(Transition transition, float duration, Easing easing)
{
	if(!mStates.empty())
		mStates.back()->Exit();

	if(!mStates.empty())
		mStates.back()->Resume();
}

bool StateMachine::Destroy()
{
  for(unsigned int i=0; i<mAllStates.size(); i++)
    if(!mAllStates[i]->Destroy())
      return false;

  return true;
}

void StateMachine::Tick(float dt)
{
  for(unsigned int i=0; i<mStates.size(); i++) {
    mStates[i]->Tick(dt);
    if(!mStates[i]->Alive) {
      mStates.erase(mStates.begin() + i);
      i--;
    }
  }
}

void StateMachine::Draw()
{
  for(unsigned int i=0; i<mStates.size(); i++)
    mStates[i]->Draw();
}

void StateMachine::RememberState(State* state)
{
  unsigned int i=0;
  for(; i<mAllStates.size(); i++)
    if(state == mAllStates[i])
      break;

  if(i == mAllStates.size())
    mAllStates.push_back(state);
}