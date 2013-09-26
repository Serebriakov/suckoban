#pragma once
#include <vector>
#include "State.h"
#include "Tween.h"
using namespace std;

namespace Gine
{
  enum Transition
  {
    TRANSITION_NONE = 0,
    TRANSITION_CROSSFADE
  };

  /// <summary>
  /// Finite-state machine for game states
  /// </summary>

  class StateMachine
  {
  public:
    ~StateMachine() {}
    
    static void Change(State* state, 
                       Transition transition = TRANSITION_NONE, float duration = 0.0f, Easing easing = NONE);
	  static void Push  (State* state, 
                       Transition transition = TRANSITION_NONE, float duration = 0.0f, Easing easing = NONE);
	  static void Pop   (Transition transition = TRANSITION_NONE, float duration = 0.0f, Easing easing = NONE);

    /// <summary> Destroys all used states </summary>
    static bool Destroy();

    static void Tick(float dt);
    static void Draw();

  private:
    static vector<State*> mStates;
    static vector<State*> mAllStates;

    static Tween mTransitionTween;
    static Transition mCurrTransition;

    /// <summary> If the given state isn't stored in mAllStates - add it there </summary>
    static void RememberState(State* state);

    StateMachine() {}
  };
}