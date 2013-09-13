#pragma once

#include "Gine.h"
#include "Info.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Model.h"
#include "Sprite.h"
#include "Input.h"

namespace Gine
{
  ID3D11Device* gDevice = 0;
  ID3D11DeviceContext* gContext = 0;
	ID3D11RenderTargetView* gRenderTargetView = 0;
	ID3D11DepthStencilView* gDepthStencilView = 0;

  Camera* gCamera = 0;
  UINT gScreenW = 0;
  UINT gScreenH = 0;

  bool Init()
  {
    Info::Log("Init Gine...");

    // Modules to refactor
    Effects::InitAll(gDevice);
    InputLayouts::InitAll(gDevice);
    RenderStates::InitAll(gDevice);

    if(!Model::Init())
    {
      Info::Fatal("Model module init failed");
      return false;
    }

    if(!Input::Init())
    {
      Info::Fatal("Input module init failed");
      return false;
    }

    if(!Sprite::Init())
    {
      Info::Fatal("Sprite module init failed");
      return false;
    }

    return true;
  }

  void Destroy()
  {
    Effects::DestroyAll();
    InputLayouts::DestroyAll();
    RenderStates::DestroyAll();
    Sprite::Destroy();
  }

  void ClearRTVAndDSV(const XMVECTORF32* color)
  {
    gContext->ClearRenderTargetView(gRenderTargetView, reinterpret_cast<const float*>(color));
    gContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
  }
}