#include "StateMachine.h"
#include "Input.h"
using namespace Gine;

vector<State*> StateMachine::mStates;
vector<State*> StateMachine::mAllStates;

void StateMachine::Change(State* state)
{
	if(!mStates.empty())
		mStates.back()->Exit();

  Input::ClearBuffers();
	mStates.push_back(state);
	mStates.back()->Enter();

  RememberState(state);
}

void StateMachine::Push(State* state)
{
	if(!mStates.empty())
		mStates.back()->Pause();
  
  Input::ClearBuffers();
	mStates.push_back(state);
	mStates.back()->Enter();

  RememberState(state);
}

void StateMachine::Pop()
{
	if(!mStates.empty())
		mStates.back()->Exit();

	if(!mStates.empty())
    mStates[mStates.size() - 2]->Resume();
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
  gContext->ClearRenderTargetView(gRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));

  for(unsigned int i=0; i<mStates.size(); i++) {
    // Draw every state to his own RTV
    ID3D11RenderTargetView* const rtv = mStates[i]->GetRTV();
    gContext->OMSetRenderTargets(1, &rtv, gDepthStencilView);
    gContext->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::MPink));
    if(i==0)   
      gContext->ClearRenderTargetView(rtv, reinterpret_cast<const float*>(&Colors::Black));

    gContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
    mStates[i]->Draw();

    // Apply post-process effect to a scene resource
    PostProcess::ApplyEffect(mStates[i]->GetPostProcess(), mStates[i]->GetSRV(), gRenderTargetView);
    
    // Render scene to screen
    //PostProcess::ApplyEffect(0, mStates[i]->GetSRV(), gRenderTargetView);
  }
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