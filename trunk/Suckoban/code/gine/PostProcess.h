#pragma once
#include "GineUtils.h"
#include "Tween.h"

namespace Gine
{
  enum PostProcessFilter
  {
    POST_PROCESS_FILTER_NONE = 0,
    POST_PROCESS_FILTER_GREYSCALE,
    POST_PROCESS_FILTER_BLUR
  };

  enum Transition
  {
    TRANSITION_NONE = 0,
    TRANSITION_FADEIN,
    TRANSITION_FADEOUT
  };

  /// <summary> 
  /// Current post-process effect inside a game state
  /// </summary>
  class PostProcess
  {
  public:
    static bool Init();
    static void Destroy();
    static void ApplyEffect(PostProcess* postProcess, ID3D11ShaderResourceView* inSRV, ID3D11RenderTargetView* outRTV);
    static ID3D11ShaderResourceView* GetSRV() { return mBufferedShaderResourceView[0]; }

    PostProcess() : mPostProcessFilter(POST_PROCESS_FILTER_NONE) {}

    void SetPostProcessFilter(PostProcessFilter postProcessFilter, Easing easing, float duration);
    void ClearPostProcessFilter(Easing easing, float duration);

    void SetTransition(Transition transition, Easing easing, float duration);

    void Tick(float dt);

  private:
    PostProcessFilter mPostProcessFilter;
    Tween mPostProcessTween;
    Transition mTransition;
    Tween mTransitionTween;

    static map<PostProcessFilter, vector<ID3DX11EffectTechnique*>> mAllFilters;
    static map<Transition, vector<ID3DX11EffectTechnique*>> mAllTransitions;

	  static ID3D11RenderTargetView*   mBufferedRenderTargetView[2];
    static ID3D11ShaderResourceView* mBufferedShaderResourceView[2];
    static ID3D11Buffer* mScreenQuadVB;
	  static ID3D11Buffer* mScreenQuadIB;

    static void CreateBackBufferViews();
    static void CreateBackBufferBuffers();

    /// <summary> Clears the backbuffer and renders to it </summary>
    static void ClearAndRenderTo(ID3D11RenderTargetView* RTV);
  };
}