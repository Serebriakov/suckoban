#pragma once
#include "GineUtils.h"
#include "Tween.h"

namespace Gine
{
  enum PostEffect
  {
    POST_EFFECT_NONE = 0,
    POST_EFFECT_GREYSCALE,
    POST_EFFECT_BLUR
  };

  /// <summary>
  /// Post processing effects
  /// </summary>

  class PostProcess
  {
  public:
    ~PostProcess() {}

    static bool Init();
    static void Destroy();
    static void Tick(float dt);

    /// <summary> Clears the backbuffer and rendeers to it </summary>
    static void RenderToBackBuffer();

    /// <summary> Sets a post process effect with a smooth transition </summary>
    static void SetEffect(PostEffect postEffect, Easing easing, float duration);
    
    /// <summary> Clears a post process effect with a smooth transition </summary>
    static void ClearEffect(Easing easing, float duration);

    /// <summary> Draws back buffer to screen and sets next rendering to screen </summary>
    static void DrawBackBuffer();

  private:
    PostProcess() {}

    static map<PostEffect, vector<ID3DX11EffectTechnique*>> mAllEfects;
    static PostEffect mCurrEffect;
    static Tween mTransition;

	  static ID3D11RenderTargetView*   mBufferedRenderTargetView[2];
    static ID3D11ShaderResourceView* mBufferedShaderResourceView[2];
    static ID3D11Buffer* mScreenQuadVB;
	  static ID3D11Buffer* mScreenQuadIB;

    static void CreateBackBufferViews();
    static void CreateBackBufferBuffers();
    static void SwapRTVBuffers();
    static void SwapSRVBuffers();

    static void SetEffect(PostEffect postEffect);

    static void RenderToScreen();
  };
}