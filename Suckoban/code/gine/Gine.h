#pragma once

#include "d3dUtil.h"
#include "d3dApp.h"
#include "Camera.h"

namespace Gine
{
  // Global DirectX data
  extern ID3D11Device* gDevice;
  extern ID3D11DeviceContext* gContext;
	extern ID3D11RenderTargetView* gRenderTargetView;
	extern ID3D11DepthStencilView* gDepthStencilView;

  /// <summary>
  /// Main camera
  /// TODO: All cameras should be in a corresponding gamestates
  ///       Fix in a project after Suckoban.
  /// </summary>
  extern Camera* gCamera;

  // Current window data
  extern UINT gScreenW;
  extern UINT gScreenH;

  /// <summary> Main engine init </summary>
  bool Init();

  /// <summary> Main engine destroy </summary>
  void Destroy();

  /// <summary> Clears main render target and depth stencil views with given color </summary>
  void ClearRTVAndDSV(const XMVECTORF32* color);

  /// <summary> Clears only depth stencil view </summary>
  void ClearDSV();
}