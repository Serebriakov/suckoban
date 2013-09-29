#pragma once
#include <vector>
#include "State.h"
#include "Tween.h"
using namespace std;

namespace Gine
{
  /// <summary>
  /// Finite-state machine for game states
  /// </summary>

  class StateMachine
  {
  public:
    ~StateMachine() {}
    
    static void Change(State* state);
	  static void Push  (State* state);
	  static void Pop   ();

    /// <summary> Destroys all used states </summary>
    static bool Destroy();

    static void Tick(float dt);
    static void Draw();

  private:
    static vector<State*> mStates;
    static vector<State*> mAllStates;

    /// <summary> If the given state isn't stored in mAllStates - add it there </summary>
    static void RememberState(State* state);

    StateMachine() {}
  };
}