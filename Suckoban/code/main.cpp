#include "gine/Gine.h"
#include "Gameplay.h"
using Gine::Info;

class Game : public D3DApp
{
public:
  Game(HINSTANCE hInstance);
  ~Game();

  State* state;

  bool Init();
  void OnResize();
  void UpdateScene(float dt);
  void DrawScene();
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd)
{
  srand(time(0));
  Game theApp(hInstance);

//  #if defined(DEBUG) | defined(_DEBUG)
//#else
//  int result = MessageBox(0, L"Start in fullscreen mode?",
//                          L"Are you ready?", 
//                          MB_YESNO | MB_ICONQUESTION);
//  if(IDYES == result)
//    theApp.SetFullscreen(true);
//#endif
//
//#if defined(DEBUG) | defined(_DEBUG)
  Info::ShowConsole();
//#endif

  if(!theApp.Init())
  {
    Info::Fatal("Application init failed");
    return 0;
  }

  Info::Log("Run...\n");
  return theApp.Run();
}

Game::Game(HINSTANCE hInstance) :
  D3DApp(hInstance),
  state(0)
{
  mMainWndCaption = L"Suckoban";
  mEnable4xMsaa = true;
  mEnableSSAO = false;
}

Game::~Game()
{
  delete state;
  md3dImmediateContext->ClearState();
  Gine::Destroy();
}

bool Game::Init()
{
  if(!D3DApp::Init()) return false;
  if(!Gine::Init()) return false;

  state = new Gameplay();
  if(!state->Init()) return false;

  return true;
}

void Game::OnResize()
{
  D3DApp::OnResize();
}

void Game::UpdateScene(float dt)
{
  if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    PostQuitMessage(0);

  Gine::Input::Tick(dt);

  state->Tick(dt);

  gCamera->UpdateViewMatrix();
}

void Game::DrawScene()
{  
  state->Draw();
  HR(mSwapChain->Present(0, 0));
}