#pragma once

#include "d3dUtil.h"
#include "d3dApp.h"
#include "Camera.h"

namespace Gine
{
  extern ID3D11Device* gDevice;
  extern ID3D11DeviceContext* gContext;
	extern ID3D11RenderTargetView* gRenderTargetView;
	extern ID3D11DepthStencilView* gDepthStencilView;

  extern Camera* gCamera;
  extern UINT gScreenW;
  extern UINT gScreenH;

  bool Init();
  void Destroy();
  void ClearRTVAndDSV(const XMVECTORF32* color);
}